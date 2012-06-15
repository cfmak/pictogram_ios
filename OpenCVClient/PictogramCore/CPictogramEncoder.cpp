#include "CPictogramEncoder.hpp"

CPictogramEncoder::CPictogramEncoder()
{
    codeSuccess = false;
}

CPictogramEncoder::CPictogramEncoder(cv::Mat img)//, cv::Mat corner)
{
    codeSuccess = false;
    aspect = float(img.size().width) / img.size().height;
    int target_width=0;
    int target_height=0;
    //downsample
    if(aspect==1.0f)
    {
        target_width = target_height = 720;
    }
    else if(aspect == 960.0f/720.0f)
    {
        target_width = 957;
        target_height = 715;
    }
    else if(aspect == 720.0f/960.0f)
    {
        target_height = 715;
        target_width = 957;
    }
//    else
//        printf("Image aspect not supported.\n");
    //preImage = img;
    cv::resize(img, preImage, cv::Size(target_width, target_height),cv::INTER_LINEAR);
    //cornerImage = corner;
}

CPictogramEncoder::~CPictogramEncoder()
{
    
}

cv::Mat CPictogramEncoder::GetOriginalImage()
{
    return preImage;
}

cv::Mat CPictogramEncoder::GetCodedImage()
{
    return postImage;
}

//cv::Mat LoadImage(const char* filepath);
//void ShowImage();

//raws is the raw number before RS and QPSK
bool CPictogramEncoder::Encode(U64 raw)
{
    unsigned char msg[9];
    for(int i=0;i<9;i++)
    {
        msg[8-i] = raw&7;
        raw = raw >> 3;
    }
    
    unsigned char rscode[21];
    rsencoder.encode(&msg[0], &rscode[0]);
    rsencoder.encode(&msg[3], &rscode[7]);
    rsencoder.encode(&msg[6], &rscode[14]);
    
    //debug
    for(int i=0;i<21;i++)
    {
        printf("%d\t", rscode[i]);
    }
    printf("\n");
    
    //2D-QPSK encode
    float QPSK_real[4] = {-1, -1, 1, 1};
    float QPSK_imag[4] = {-1, 1, -1, 1};
    
    float fft_real[11][11];
    float fft_imag[11][11];
    memset(fft_real, 0, sizeof(float)*11*11);
    memset(fft_imag, 0, sizeof(float)*11*11);
    int ij=1;
    int ik=1;
    for(int ii=0;ii<21;ii++)
    {
        fft_real[ij][ik] =  QPSK_real[rscode[ii]&3];
        fft_imag[ij][ik] =  QPSK_imag[rscode[ii]&3];
        if(rscode[ii]>>2 == 0)
        {
//            B = fliplr(A);
//            C = flipud(conj(A));
//            D = fliplr(C);
            //B
            fft_real[ij][11-ik] =  fft_real[ij][ik];
            fft_imag[ij][11-ik] =  fft_imag[ij][ik];
            
            //C
            fft_real[11-ij][ik] = fft_real[ij][ik];
            fft_imag[11-ij][ik] = -fft_imag[ij][ik];
            
            //D
            fft_real[11-ij][11-ik] = fft_real[11-ij][ik];
            fft_imag[11-ij][11-ik] = fft_imag[11-ij][ik];
        }
        else
        {
//            B = - fliplr(A);
//            C = flipud(conj(-A));
//            D = -fliplr(C);
            //B
            fft_real[ij][11-ik] =  -fft_real[ij][ik];
            fft_imag[ij][11-ik] =  -fft_imag[ij][ik];
            
            //C
            fft_real[11-ij][ik] = -fft_real[ij][ik];
            fft_imag[11-ij][ik] = fft_imag[ij][ik];
            
            //D
            fft_real[11-ij][11-ik] = -fft_real[11-ij][ik];
            fft_imag[11-ij][11-ik] = -fft_imag[11-ij][ik];
        }
        ij = ij+1;
        if(ij == 6)
        {
            ik = ik+1;
            ij = 1;
        }
    }
    
    for(int ii=0;ii<11;ii++)
    {
        for(int ij=0;ij<11;ij++)
        {
            printf("%.0f+%.0fi ", fft_real[ii][ij], fft_imag[ii][ij]);
        }
        printf("\n");
    }
    
    cv::Mat fftr(11,11, CV_32FC1, fft_real);
    cv::Mat ffti(11,11, CV_32FC1, fft_imag);
    cv::Mat planes[] = {fftr, ffti};
    
    cv::Mat fft;
    cv::merge(planes, 2, fft);
    
    cv::Mat ifft;
    cv::idft(fft,ifft);
    
    //debug - print spatial signal
//    std::cout<<ifft<<std::endl;
    
    //now deal with the image
    U32 blkSize1=floor(preImage.size().width/11.0f);
    U32 blkSize2=floor(preImage.size().height/11.0f);
    
    U32 blkArea = blkSize1 * blkSize2;
//    U32 maxBlockColor = 255*blkArea;
    U32 maxSignalColor = 89*blkArea;
    //U32 maxSignalColor = 0*blkArea;
    U32 maxBackgroundColor = 38*blkArea;
    U32 base = 128*blkArea;
    
    std::vector<cv::Mat> splitMat(2);
    cv::split(ifft, splitMat);
//    std::cout<<splitMat[0]<<std::endl;
    
    //normalize!
    //std::cout<<splitMat[0];
//    printf("max:%f\tmaxSignal:%u\tnormConst:%f\n", maxWithinMat(splitMat[0]), maxSignalColor,maxSignalColor/maxWithinMat(splitMat[0]));
    splitMat[0] = splitMat[0]*(maxSignalColor/abs(maxWithinMat(splitMat[0])));
//    std::cout<<splitMat[0]<<std::endl;
    
    //original image statistics
    for(int ii=0;ii<11;ii++)
    {
        for(int ij=0;ij<11;ij++)
        {
            planes[0].at<float>(ii,ij)=SumMegapixel(preImage, ii, ij, blkSize1, blkSize2)-int(base);
        }
    }
    fftr = planes[0];
//    std::cout<<planes[0]<<std::endl;
    planes[1] = cv::Mat::zeros(cv::Size(11,11), CV_32F);
    cv::merge(planes, 2, ifft);
    cv::dft(ifft,fft);
    for(int ii=1;ii<11;ii++)
    {
        for(int ij=1;ij<11;ij++)
        {
            fft.at<cv::Vec2f>(ii,ij)=cv::Vec2f(0,0);
        }
    }
    cv::idft(fft,ifft, cv::DFT_SCALE);
    cv::split(ifft, planes);
    float amax = abs(maxWithinMat(planes[0]));
    if(amax > maxBackgroundColor)
        splitMat[1] = planes[0]*(maxBackgroundColor/amax);
//    std::cout<<splitMat[1]<<std::endl;
    
    //splitMat[0] contains spatial signal
    //splitMat[1] contains spatial background
    //add to form spatial target
    ffti=splitMat[0]+splitMat[1];
    
    //ffti contains spatial target
    //fftr contains spatial original
//    postImage = preImage;
 //   CV_8U
//    printf("depth=%d channel=%d\n",preImage.depth(), preImage.channels());
//    std::cout<<postImage<<std::endl;
    cv::Mat channels[3];
    cv::split(preImage, channels);
//    std::cout<<channels[0]<<std::endl;
    
    //postImage.at<postImage.type()>(717,65)[0] = 0;
    for(int ii=0;ii<11;ii++)
    {
        for(int ij=0;ij<11;ij++)
        {
            int delta = (int)((ffti.at<float>(ii,ij)-fftr.at<float>(ii,ij)) / (int)blkArea);
            printf("%d\t",delta);
            //printf("%d ",delta);
            for(int ik=ii*blkSize1; ik<(ii+1)*blkSize1; ik++)
            {
                for(int il=ij*blkSize2; il<(ij+1)*blkSize2; il++)
                {
//                    printf("%d %d\n", ik,il);
                    int tmp = (int)channels[0].at<unsigned char>(il,ik) + delta;
                    if(tmp > 255)
                        channels[0].at<unsigned char>(il,ik) = 255;
                    else if(tmp < 0)
                        channels[0].at<unsigned char>(il,ik) = 0;
                    else
                        channels[0].at<unsigned char>(il,ik) = tmp;
                    
                    tmp = (int)channels[1].at<unsigned char>(il,ik) + delta;
                    if(tmp > 255)
                        channels[1].at<unsigned char>(il,ik) = 255;
                    else if(tmp < 0)
                        channels[1].at<unsigned char>(il,ik) = 0;
                    else
                        channels[1].at<unsigned char>(il,ik) = tmp;
                    
                    tmp = (int)channels[2].at<unsigned char>(il,ik) + delta;
                    if(tmp > 255)
                        channels[2].at<unsigned char>(il,ik) = 255;
                    else if(tmp < 0)
                        channels[2].at<unsigned char>(il,ik) = 0;
                    else
                        channels[2].at<unsigned char>(il,ik) = tmp;
//                    channels[1].at<unsigned char>(ik,il) += delta;
//                    channels[2].at<unsigned char>(ik,il) += delta;
                    
//                    postImage.at<cv::Vec3b>(ik,il)[0] = preImage.at<cv::Vec3b>(ik,il)[0] + delta;
//                    postImage.at<cv::Vec3b>(ik,il)[1] = preImage.at<cv::Vec3b>(ik,il)[1] + delta;
//                    postImage.at<cv::Vec3b>(ik,il)[2] = preImage.at<cv::Vec3b>(ik,il)[2] + delta;
//                    printf("%d %d %d %d\n", ii, ij, ik,il);

                }
            }
            
//    D(bx*blkSize1+1 : (bx+1)*blkSize1, by*blkSize2+1:(by+1)*blkSize2,:) = ...
//    A_resize(bx*blkSize1+1 : (bx+1)*blkSize1, by*blkSize2+1:(by+1)*blkSize2, :) - ...
//    delta;
        }
        printf("\n");
    }
    cv:merge(channels, 3, postImage);
    
    return true;
}

float CPictogramEncoder::maxWithinMat(cv::Mat& x)
{
    cv::Mat absx = cv::abs(x);
    float max = -1000000.0f;
    for(int i=0;i<absx.size().width;i++)
    {
        for(int j=0;j<absx.size().height;j++)
        {
            float a = x.at<float>(i,j);
            if(a > max)
                max = a;
        }
    }
    return max;
}

int CPictogramEncoder::SumMegapixel(cv::Mat& img, int mpx, int mpy, float megapixel_width, float megapixel_height)
{
    U32 acc = 0;
    //Mat gray(image_gray);
    
    for(int i=floor(mpx*megapixel_height);i<floor((mpx+1)*megapixel_height); i++)
    {
        for(int j=floor(mpy*megapixel_width);j<floor((mpy+1)*megapixel_width); j++)
        {
            acc+= int(img.at<unsigned char>(i,j));
        }
    }
    
    return acc;
}