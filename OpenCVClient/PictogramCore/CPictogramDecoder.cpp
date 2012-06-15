#include "CPictogramDecoder.hpp"

#include "FastMatchTemplate.h"

//#include <opencv2/opencv.hpp>

CPictogramDecoder::CPictogramDecoder()
{
    corrected = false;
    corrected_width = corrected_height = 0;
}

CPictogramDecoder::CPictogramDecoder(cv::Mat img, cv::Mat tlCorner, cv::Mat trCorner, cv::Mat blCorner, cv::Mat brCorner)
{
    tl = tlCorner;
    tr = trCorner;
    bl = blCorner;
    br = brCorner;
    image = img;
    corrected = false;
    corrected_width = corrected_height = 0;
}

CPictogramDecoder::~CPictogramDecoder()
{
    //if(bbox)
    //    cvFree(&bbox);
    //if(image_gray)
    //    cvReleaseImage(&image_gray);
}

cv::Mat CPictogramDecoder::GetImage()
{
    return image;
}

//cv::Mat CPictogram::LoadImage(const char* filepath)
//{
//    image = imread(filepath);
//    //image_gray= 0;
//    corrected = false;
//    
//    return image;
//}

/*
bool CPictogram::FindBBoxFlann(int* ret)
{
    int minHessian = 400;
    
    SurfFeatureDetector detector( minHessian );
    
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    
    detector.detect( tl, keypoints_1 );
    detector.detect( image, keypoints_2 );
    
    //-- Step 2: Calculate descriptors (feature vectors)
    SurfDescriptorExtractor extractor;
    
    Mat descriptors_1, descriptors_2;
    
    extractor.compute( tl, keypoints_1, descriptors_1 );
    extractor.compute( image, keypoints_2, descriptors_2 );
    
    //-- Step 3: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );
    
    double max_dist = 0; double min_dist = 100;
    
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_1.rows; i++ )
    { 
        double dist = matches[i].distance;
        printf("-- dist : %f \n", dist );
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    
    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );
    
    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist )
    //-- PS.- radiusMatch can also be used here.
    std::vector< DMatch > good_matches;
    
    for( int i = 0; i < descriptors_1.rows; i++ )
    { 
        if( matches[i].distance < 2*min_dist )
        { 
            good_matches.push_back( matches[i]); 
        }
    }
    printf("number of good matches = %d\n", good_matches.size());
    Mat img_matches;
    drawMatches( tl, keypoints_1, image, keypoints_2,
                good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    image = img_matches;
    return true;
}
*/

bool CPictogramDecoder::FindBBox(int* ret, float corrThreshold)
{
    bool cornerTempFound = false;
    if(corrected) return false;
    int r = 0;
    
    Point tlf, trf, blf, brf;
    
    cv::equalizeHist(image, eqlize);
    
    for(int row = 0; row < eqlize.rows; ++row) {
        unsigned char* inp  = eqlize.ptr<unsigned char>(row);
        for (int col = 0; col < eqlize.cols; ++col) {
            if (*inp < 50) {
                *inp = 0;
            } 
            else if (*inp > 255-50)
            {
                *inp = 255;
            }
            else
            {
                *inp = 128;
            }
            *inp++;
        }
    }
    
    vector<Point>  foundList;
    vector<double> confidencesList;
    CPictogramDecoder::FastMatchTemplate(eqlize, tl, corrThreshold, &foundList, &confidencesList);
    if(foundList.size()>=4)
    {
        double conf=0;
        confList.resize(4);
        FindNearestPoint(cv::Point(0,0), &foundList, &confidencesList, &tlf, &conf);
        confList[0] = conf;
        FindNearestPoint(cv::Point(image.size().width,0), &foundList, &confidencesList, &trf, &conf);
        confList[1] = conf;
        FindNearestPoint(cv::Point(0,image.size().height), &foundList, &confidencesList, &blf, &conf);
        confList[2] = conf;
        FindNearestPoint(cv::Point(image.size().width,image.size().height), &foundList, &confidencesList, &brf, &conf);
        confList[3] = conf;
        
        //check the four corners are four distinct points
        if(tlf.x==trf.x && tlf.y==trf.y) return false;
        if(tlf.x==blf.x && tlf.y==blf.y) return false;
        if(trf.x==brf.x && trf.y==brf.y) return false;
        if(blf.x==brf.x && blf.y==brf.y) return false;
        if(tlf.x==brf.x && tlf.y==brf.y) return false;
        if(trf.x==blf.x && trf.y==blf.y) return false;
        
        
//cv::circle(image, tlf, 15, Scalar::all(0));
//cv::circle(image, trf, 15, Scalar::all(0));
//cv::circle(image, blf, 15, Scalar::all(0));
//cv::circle(image, brf, 15, Scalar::all(0));
//printf("%d %d\n", tlf.x, tlf.y);
//printf("%d %d\n", trf.x, trf.y);
//printf("%d %d\n", blf.x, blf.y);
//printf("%d %d\n", brf.x, brf.y);
        //return true;
        cornerTempFound = true;
    }
    //return false;
    
    if(!cornerTempFound)
    {
        return false;
    }
    
    cv::Mat eig_img;
    cv::Mat temp_img;
    vector<Point2f> corners;
    
    int i, max_corner_count = 150;
    int idx;
    int x;
    int y;
    
    float cornerThres = 0.4f;
    
    //TL Corner    
    x = tlf.x;
    y = tlf.y;
    cv::Mat roi = image(cv::Rect(x, y, 10, 10));
    
    cv::goodFeaturesToTrack(roi, corners, max_corner_count, cornerThres, 1);
    
    // If we find multiple corners, take the BR corner
    idx = 0;
    corners.push_back(Point(tlf.x-x, tlf.y-y));
    for (i = 0; i < corners.size(); i++)
    {
        if(i==0 || (corners[i].x > corners[idx].x && corners[i].y > corners[idx].y))
        {
            idx = i;
        }
    }
    if(corners.size() > 0)
    {
        bbox[0].x = corners[idx].x + x;
        bbox[0].y = corners[idx].y + y;
cv::circle(image, bbox[0], 5, Scalar::all(0));
    }
    
    
    //TR Corner
    x = trf.x-10;
    y = trf.y;
    roi = image(cv::Rect(x, y, 10, 10));
    
    cv::goodFeaturesToTrack(roi, corners, max_corner_count, cornerThres, 1);
    
    // If we find multiple corners, take the BL corner
    idx=0;
    corners.push_back(Point(trf.x-x, trf.y-y));
    for (i = 0; i < corners.size(); i++)
    {
        if(i==0 || (corners[i].x < corners[idx].x && corners[i].y > corners[idx].y))
        {
            idx = i;
        }
    }
    if(corners.size() > 0)
    {
        bbox[1].x = corners[idx].x + x;
        bbox[1].y = corners[idx].y + y;
cv::circle(image, bbox[1], 5, Scalar::all(0));
    }
    //else
    //    return false;//we can "guess" the point instead of returning false
    
    //BL Corner
    x = blf.x;
    y = blf.y-10;
    roi = image(cv::Rect(x, y, 10, 10 ));
    
    cv::goodFeaturesToTrack(roi, corners, max_corner_count, cornerThres, 1);
    
    // If we find multiple corners, take the TR corner
    idx=0;
    corners.push_back(Point(blf.x-x, blf.y-y));
    for (i = 0; i < corners.size(); i++)
    {
        if(i==0 || (corners[i].x > corners[idx].x && corners[i].y < corners[idx].y))
        {
            idx = i;
        }
    }
    if(corners.size() > 0)
    {
        bbox[2].x = corners[idx].x + x;
        bbox[2].y = corners[idx].y + y;
cv::circle(image, bbox[2], 5, Scalar::all(0));
    }
    
    
    //BR Corner
    x = brf.x-10;
    y = brf.y-10;
    roi = image(cv::Rect(x, y, 10, 10 ));
    
    cv::goodFeaturesToTrack(roi, corners, max_corner_count, cornerThres, 1);
    
    // If we find multiple corners, take the TL corner
    idx=0;
    corners.push_back(Point(brf.x-x, brf.y-y));
    for (i = 0; i < corners.size(); i++)
    {
        if(i==0 || (corners[i].x < corners[idx].x && corners[i].y < corners[idx].y))
        {
            idx = i;
        }
    }
    if(corners.size() > 0)
    {
        bbox[3].x = corners[idx].x + x;
        bbox[3].y = corners[idx].y + y;
cv::circle(image, bbox[3], 5, Scalar::all(0));
    }
    
    return true;
}

bool CPictogramDecoder::FindNearestPoint(cv::Point ref, std::vector<cv::Point>*  foundPointsList, 
                                  std::vector<double>*  confidencesList, cv::Point* pt, double* conf)
{
    if(foundPointsList->size() == 0)
        return false;
    *pt = (*foundPointsList)[0];
    *conf = (*confidencesList)[0];
    float dx = pt->x-ref.x;
    float dy = pt->y-ref.y;
    float norm2 = dx*dx + dy*dy;
    //printf("ref %d %d\n", ref.x, ref.y);
    //printf("px %d, py %d, norm2 %f\n", (*foundPointsList)[0].x, (*foundPointsList)[0].y, norm2);
    for(int i=1;i<foundPointsList->size(); i++)
    {
        dx = (*foundPointsList)[i].x-ref.x;
        dy = (*foundPointsList)[i].y-ref.y;
        float currentNorm2 = dx*dx + dy*dy;
        //printf("px %d, py %d, currentnorm2 %f\n", (*foundPointsList)[i].x, (*foundPointsList)[i].y, currentNorm2);
        if(currentNorm2 < norm2)
        {
            *pt = (*foundPointsList)[i];
            *conf = (*confidencesList)[i];
            norm2 = currentNorm2;
        }
    }
    return true;
}

bool CPictogramDecoder::FastMatchTemplate(cv::Mat subImage, cv::Mat target, float corrThreshold, 
                                   std::vector<cv::Point>*  foundPointsList,
                                   std::vector<double>* confidencesList)
{
    Mat source = subImage;
    //Mat target = imread(filename, 1);
    
    //vector<Point> foundPointsList;
    //vector<double> confidencesList;
    
    // start the timer
//    ( const Mat&      source,
//     const Mat&      target,
//     vector<Point>*  foundPointsList,
//     vector<double>* confidencesList,
//     int             matchPercentage,
//     bool            findMultipleTargets,
//     int             numMaxima,
//     int             numDownPyrs,
//     int             searchExpansion )
    if(!CFastMatchTemplate::FastMatchTemplate(source,
                                              target,
                                              foundPointsList,
                                              confidencesList,
                                              corrThreshold,
                                              true,
                                              5,
                                              1))
    {
//printf("\nERROR: Fast match template failed.\n");
        return false;
    }
//printf("number of matches = %lu\n", confidencesList->size());
    for(int i=0;i<confidencesList->size();i++)
    {
        cv::circle(source, (*foundPointsList)[i], 15, Scalar::all(128));
        //printf("conf=%f\n", confidencesList[i]);
    }
    return (foundPointsList->size() >= 4);
}



bool CPictogramDecoder::CorrectPerspective()
{
    if(corrected || !bbox) return false;
    
    corrected_width = abs(bbox[1].x - bbox[0].x);
    corrected_height = abs(bbox[2].y - bbox[0].y);
    
    if(corrected_width<50 || corrected_height<50) return false;
    if(abs(bbox[3].x - bbox[2].x)<50 || abs(bbox[1].y - bbox[3].y) < 50) return false;
    
    Point2f original[4];
    original[0].x = 0;                  original[0].y = 0;
    original[1].x = corrected_width;    original[1].y = 0;
    original[2].x = 0;                  original[2].y = corrected_height;
    original[3].x = corrected_width;    original[3].y = corrected_height;
    
    cv::Mat perspective = cv::getPerspectiveTransform(original, bbox);
    cout << "P = "<< endl << " "  << perspective << endl;
    
    //take out the xy-mapping part, and check SVD, expect it to be "almost" an ortho matrix (plus some skewing etc)
    cv::Mat rot = perspective(Rect(0,0,2,2));
    //cout << "R = "<<endl<<" " <<rot <<endl<<endl;
    
    cv::SVD rotsvd(rot);
    //cout << "sigma = "<<endl<<" " <<rotsvd.w <<endl<<endl;
    if(rotsvd.w.at<double>(0, 0)>1.2 || rotsvd.w.at<double>(0, 1) < 0.8)
    {
        cout<<"too skewed"<<endl<<endl;
        return false; //too skewed
    }
    //cout << "U = "<<endl<<" " << rotsvd.u <<endl<<endl;
    //cout << "V = "<<endl<<" " << rotsvd.vt <<endl<<endl;
    double forwardr = asin(rotsvd.u.at<double>(0,1));
    double backwardr = asin(rotsvd.vt.at<double>(0,1));
    //cout << "forwardr = "<<forwardr<<endl<<"backwardr = "<<backwardr<<endl<<endl;
    if(abs(forwardr)>=M_PI_2 || abs(backwardr)>=M_PI_2) 
    {
        cout << "too much individual rotation"<<endl<<endl;
        return false; //too much rotation
    }
    if(abs(forwardr + backwardr) > 0.2) 
    {
        cout << "too much net rotation" <<endl<<endl;
        return false; //too much net rotation
    }
    
    //passed the SVD test, now correct
    cv::Mat perspectiveInv = perspective.inv(DECOMP_SVD); //pseudo inverse
    cv::warpPerspective(image, undistorted, perspectiveInv, Size(corrected_width, corrected_height)); //apply matrix to undistort with resampling
    
    corrected = true;
    return true;
}

cv::Mat CPictogramDecoder::GetUndistorted()
{
    return undistorted;
}

bool CPictogramDecoder::Decode(unsigned int& out)
{
    if(!corrected) return false;
    //image_gray = cvCreateImage( cvGetSize( image ), 8, 1 );
    //cvtColor( image, image_gray, CV_RGB2GRAY );
    
    float megapixel_width = corrected_width / 11.0f;
    float megapixel_height = corrected_height / 11.0f;
    U32 megapixel_max = (255*megapixel_width*megapixel_height);
    
    float table[11][11];
    memset(table, 0, 11*11*sizeof(float));
    
    for(int i=0;i<11;i++)
    {
        for(int j=0;j<11;j++)
        {
            table[i][j] = float(SumMegapixel(i, j, megapixel_width, megapixel_height)) / float(megapixel_max);
            //printf("%f\t", table[i][j]);
        }
        //printf("\n");
    }
    
    Mat MegaPixel(11,11, CV_32FC1, table); //put the table into a Mat
    
    Mat fft2MegaPixel;      //this will hold the fft2 result
    Mat planes[] = {Mat_<float>(MegaPixel), Mat::zeros(MegaPixel.size(), CV_32F)};
    merge(planes, 2, fft2MegaPixel);         // Add the imaginary part (= zero matrix of same size)
    
    dft(fft2MegaPixel,fft2MegaPixel);
    
//    for(int i=1;i<=5;i++)
//    {
//        for(int j=1;j<=5;j++)
//        {
//            Vec2f& symbol = fft2MegaPixel.at<Vec2f>(i, j);
////            printf("%d+%di\t", symbol[0]>0?1:-1, symbol[1]>0?1:-1);
//            printf("%.2f+%f.2i\t", symbol[0], symbol[1]);
//        }
//        printf("\n");
//    }
    
    unsigned char rscode[25];
//    U64 result = 0;
    int ii=0;
    for(int i=1;i<=5;i++)
    {
        for(int j=1;j<=5;j++)
        {
            U64 a = 0;
            Vec2f& symbol = fft2MegaPixel.at<Vec2f>(j, i);

            if(symbol[0]>0 && symbol[1]>0)
                a = 3;
            else if(symbol[0]>0 && symbol[1]<0)
                a = 2;
            else if(symbol[0]<0 && symbol[1]>0)
                a = 1;
            else
                a = 0;
            
            Vec2f& symbolB = fft2MegaPixel.at<Vec2f>(j, 11-i);
            if(((symbolB[0]>0 && symbol[0]<0)||(symbolB[0]<0 && symbol[0]>0)) &&
               ((symbolB[1]>0 && symbol[1]<0)||(symbolB[1]<0 && symbol[1]>0)))
               a = a|4;
            rscode[ii++] = a;
            
//            a = a << ((i-1)*5 + j-1)*3;
//            result = result | a;
            
        }
    }
//    printf("\nrscode:\n");
//    for(ii=0;ii<25;ii++)
//        printf("%d\t", rscode[ii]);
    //result = result & 0x7fffffffffffffff; //take lower 32 bits
    //unsigned int hi = (unsigned int)(result>>32);
    //unsigned int lo = (unsigned int)(result&0xffffffff);
    //printf("result = %llu\n", result);
    
    unsigned char dec[9];
    if(rsdecoder.decode(&rscode[0]))
        rsdecoder.getDecodedMessage(&dec[0]);
    else
        return false;
    if(rsdecoder.decode(&rscode[7]))
        rsdecoder.getDecodedMessage(&dec[3]);
    else
        return false;
    if(rsdecoder.decode(&rscode[14]))
        rsdecoder.getDecodedMessage(&dec[6]);
    else 
        return false;
    
    out = 0;
    for(ii=0;ii<9;ii++)
    {
        out = out << 3;
        out |= dec[ii];
        
    }
//    printf("\nMessage:\t");
//    for(ii=0;ii<9;ii++)
//        printf("%d\t", dec[ii]);
//    printf("\nout: %d", out);
    return true;
}

U32 CPictogramDecoder::SumMegapixel(int mpx, int mpy, float megapixel_width, float megapixel_height)
{
    if(!corrected) return 0;
    U32 acc = 0;
    //Mat gray(image_gray);
    
    for(int i=floor(mpx*megapixel_height);i<floor((mpx+1)*megapixel_height); i++)
    {
        for(int j=floor(mpy*megapixel_width);j<floor((mpy+1)*megapixel_width); j++)
        {
            acc+= undistorted.at<unsigned char>(i,j);
        }
    }
    
    return acc;
}
