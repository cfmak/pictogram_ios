#include "CPictogram.hpp"

#include "FastMatchTemplate.h"

//#include <opencv2/opencv.hpp>

CPictogram::CPictogram()
{
    corrected = false;
    corrected_width = corrected_height = 0;
}

CPictogram::CPictogram(cv::Mat img, cv::Mat tlCorner, cv::Mat trCorner, cv::Mat blCorner, cv::Mat brCorner)
{
    tl = tlCorner;
    tr = trCorner;
    bl = blCorner;
    br = brCorner;
    image = img;
    corrected = false;
    corrected_width = corrected_height = 0;
}

CPictogram::~CPictogram()
{
    //if(bbox)
    //    cvFree(&bbox);
    //if(image_gray)
    //    cvReleaseImage(&image_gray);
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
void CPictogram::ShowImage()
{
    if(!image) return;
    
    cvNamedWindow("Image");
    
    IplImage* out = cvCreateImage( cvSize(image->width/2,image->height/2), image->depth, image->nChannels );
        
    // Reduce the image by 2
    cvPyrDown( image, out );
        
    //cvCanny( out, out, 60, 100, 3 );
    cvShowImage("Image", out);
    cvWaitKey(0);
    cvReleaseImage( &out );
    cvDestroyWindow( "Image" );
    
}*/

/*
void CPictogram::ShowContour()
{
    if(!image) return;
    int		thresh = 120;

    if( storage == NULL ){
		storage = cvCreateMemStorage();
	} else {
		cvClearMemStorage( storage );
	}
    
	CvSeq* contours = 0;
	cvCvtColor( image, gray, CV_BGR2GRAY );
	cvThreshold( gray, gray, thresh, 255, CV_THRESH_BINARY );
	cvFindContours( gray, storage, &contours );
	cvZero( gray );
	if( contours ){
		cvDrawContours(
                       gray,
                       contours,
                       cvScalarAll(255),
                       cvScalarAll(255),
                       100 );
	}
	cvShowImage( "Contours", gray );
}
 */

bool CPictogram::FindBBox(int* ret, float corrThreshold)
{
    if(corrected) return false;
    int r = 0;
    
    Point tlf, trf, blf, brf;
    
    cv::Mat eqlize;
    cv::equalizeHist(image, image);
    
    for(int row = 0; row < image.rows; ++row) {
        unsigned char* inp  = image.ptr<unsigned char>(row);
        for (int col = 0; col < image.cols; ++col) {
            if (*inp < 30) {
                *inp = 0;
            } 
            else if (*inp > 255-30)
            {
                *inp = 255;
            }
            else
            {
                *inp = 255;
            }
            *inp++;
        }
    }
    /*
    cv::Mat tlROI = image(Rect(0,0,image.size().width/2, image.size().height/2));
    if(CPictogram::FastMatchTemplate(tlROI, tl, corrThreshold, &tlf.x, &tlf.y))
    {
        //cv::circle(tlROI, tlf, 5, Scalar::all(255));
        r |= 1;
    }
    cv::Mat trROI = image(Rect(image.size().width/2,0, image.size().width/2-1, image.size().height/2));
    if(CPictogram::FastMatchTemplate(trROI, tr, corrThreshold, &trf.x, &trf.y))
    {
        //cv::circle(trROI, trf, 5, Scalar::all(255));
        r |= 2;
    }
    cv::Mat blROI = image(Rect(0,image.size().height/2, image.size().width/2, image.size().height/2-1));
    if(CPictogram::FastMatchTemplate(blROI, bl, corrThreshold, &blf.x, &blf.y))
    {
        //cv::circle(blROI, blf, 5, Scalar::all(255));
        r |= 4;
    }
    cv::Mat brROI = image(Rect(image.size().width/2,image.size().height/2, image.size().width/2-1, image.size().height/2-1));
    if(CPictogram::FastMatchTemplate(brROI, br, corrThreshold, &brf.x, &brf.y))
    {
        //cv::circle(brROI, brf, 5, Scalar::all(255));
        r |= 8;
    }
    if(ret!=0)
        *ret = r;
    
    if(r != (1|2|4|8))
        return false;*/
    
    vector<Point>  foundPointsList;
    vector<double> confidencesList;
    CPictogram::FastMatchTemplate(image, tl, corrThreshold, &foundPointsList, &confidencesList);
    if(foundPointsList.size()>=4)
    {
        FindNearestPoint(cv::Point(0,0), &foundPointsList, &tlf);
        FindNearestPoint(cv::Point(image.size().width,0), &foundPointsList, &trf);
        FindNearestPoint(cv::Point(0,image.size().height), &foundPointsList, &blf);
        FindNearestPoint(cv::Point(image.size().width,image.size().height), &foundPointsList, &brf);
        
        cv::circle(image, tlf, 5, Scalar::all(255));
        cv::circle(image, trf, 5, Scalar::all(255));
        cv::circle(image, blf, 5, Scalar::all(255));
        cv::circle(image, brf, 5, Scalar::all(255));
        printf("%d %d\n", tlf.x, tlf.y);
        printf("%d %d\n", trf.x, trf.y);
        printf("%d %d\n", blf.x, blf.y);
        printf("%d %d\n", brf.x, brf.y);
    }
    
    //cv::Mat g;
    //cvtColor( image, g, CV_RGB2GRAY );
    //IplImage g1 = g;
    //IplImage* gray = &g1;
    
//    IplImage g1 = image;
//    IplImage* gray = &g1;
//    
//    //TL Corner
//    CvRect roi = cvRect(tlf.x+1, tlf.y+1, 100, 100);
//    cvSetImageROI( gray, roi);
//
//    int i, corner_count = 150;
//    IplImage *eig_img, *temp_img;
//    CvPoint2D32f *corners;
//    
//    eig_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    temp_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    corners = (CvPoint2D32f *) cvAlloc (corner_count * sizeof (CvPoint2D32f));
//    
//    cvGoodFeaturesToTrack (gray, eig_img, temp_img, corners, &corner_count, 0.1, 15);
//    cvFindCornerSubPix (gray, corners, corner_count,
//                        cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
//    
//    // If we find multiple corners, take the TL corner
//    int idx = 0;
//    for (i = 0; i < corner_count; i++)
//    {
//        if(i==0 || (corners[i].x < corners[idx].x && corners[i].y < corners[idx].y))
//        {
//            idx = i;
//        }
//    }
//    if(corner_count > 0)
//    {
//        bbox[0].x = corners[idx].x + tlf.x+1;
//        bbox[0].y = corners[idx].y + tlf.y+1;
//        //cvCircle (gray, cvPointFrom32f (corners[idx]), 3, CV_RGB (255, 0, 0), 2);
//    }
//    else
//        return false;//we can "guess" the point instead of returning false
//
//    cvResetImageROI(gray);
//    cvReleaseImage (&eig_img);
//    cvReleaseImage (&temp_img);
//    
//    //TR Corner
//    roi = cvRect(trf.x-101, trf.y+1, 100, 100);
//    cvSetImageROI( gray, roi);
//    
//    corner_count = 150;
//    
//    eig_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    temp_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    
//    cvGoodFeaturesToTrack (gray, eig_img, temp_img, corners, &corner_count, 0.1, 15);
//    cvFindCornerSubPix (gray, corners, corner_count,
//                        cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
//    
//    // If we find multiple corners, find the TR one
//    idx = 0;
//    for (i = 0; i < corner_count; i++)
//    {
//        if(i==0 || (corners[i].x > corners[idx].x && corners[i].y < corners[idx].y))
//        {
//            idx = i;
//        }
//    }
//    if(corner_count > 0)
//    {
//        bbox[1].x = corners[idx].x + trf.x-101;
//        bbox[1].y = corners[idx].y + trf.y+1;
//        //cvCircle (gray, cvPointFrom32f (corners[idx]), 3, CV_RGB (255, 0, 0), 2);
//    }
//    else
//        return false;//we can "guess" the point instead of returning false
//    
//    cvResetImageROI(gray);
//    cvReleaseImage (&eig_img);
//    cvReleaseImage (&temp_img);
//    
//    //BL Corner
//    roi = cvRect(blf.x+1, blf.y-101, 100, 100);
//    cvSetImageROI( gray, roi);
//    
//    corner_count = 150;
//    
//    eig_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    temp_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    
//    cvGoodFeaturesToTrack (gray, eig_img, temp_img, corners, &corner_count, 0.1, 15);
//    cvFindCornerSubPix (gray, corners, corner_count,
//                        cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
//    
//    // If we find multiple corners, find the BL one
//    idx = 0;
//    for (i = 0; i < corner_count; i++)
//    {
//        if(i==0 || (corners[i].x < corners[idx].x && corners[i].y > corners[idx].y))
//        {
//            idx = i;
//        }
//    }
//    if(corner_count > 0)
//    {
//        bbox[2].x = corners[idx].x + blf.x+1;
//        bbox[2].y = corners[idx].y + blf.y-101;
//        //cvCircle (gray, cvPointFrom32f (corners[idx]), 3, CV_RGB (255, 0, 0), 2);
//    }
//    else
//        return false;//we can "guess" the point instead of returning false
//    
//    cvResetImageROI(gray);
//    cvReleaseImage (&eig_img);
//    cvReleaseImage (&temp_img);
//    
//    
//    //BR corner
//    roi = cvRect(brf.x-101, brf.y-101, 100, 100);
//    cvSetImageROI( gray, roi);
//    
//    corner_count = 150;
//    
//    eig_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    temp_img = cvCreateImage (cvGetSize (gray), IPL_DEPTH_32F, 1);
//    
//    cvGoodFeaturesToTrack (gray, eig_img, temp_img, corners, &corner_count, 0.1, 15);
//    cvFindCornerSubPix (gray, corners, corner_count,
//                        cvSize (3, 3), cvSize (-1, -1), cvTermCriteria (CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
//    
//    // If we find multiple corners, find the BR one
//    idx = 0;
//    for (i = 0; i < corner_count; i++)
//    {
//        if(i==0 || (corners[i].x > corners[idx].x && corners[i].y > corners[idx].y))
//        {
//            idx = i;
//        }
//    }
//    if(corner_count > 0)
//    {
//        bbox[3].x = corners[idx].x + brf.x-101;
//        bbox[3].y = corners[idx].y + brf.y-101;
//        //cvCircle (gray, cvPointFrom32f (corners[idx]), 3, CV_RGB (255, 0, 0), 2);
//    }
//    else
//        return false;//we can "guess" the point instead of returning false
//    
//    //cvReleaseImage (&gray);
//    cvReleaseImage (&eig_img);
//    cvReleaseImage (&temp_img);
//    cvFree(&corners);       
    
    return true;
}

bool CPictogram::FindNearestPoint(cv::Point ref, std::vector<cv::Point>*  foundPointsList, cv::Point* pt)
{
    if(foundPointsList->size() == 0)
        return false;
    pt->x = (*foundPointsList)[0].x;
    pt->y = (*foundPointsList)[0].y;
    float norm2 = (pt->x-ref.x)*(pt->x-ref.x)+(pt->y-ref.y)*(pt->y-ref.y);
    for(int i=1;i<foundPointsList->size(); i++)
    {
        float currentNorm2 = ((*foundPointsList)[i].x-ref.x)*((*foundPointsList)[i].x-ref.x)+
                             ((*foundPointsList)[i].y-ref.y)*((*foundPointsList)[i].y-ref.y);
        if(currentNorm2 < norm2)
        {
            pt->x = (*foundPointsList)[i].x;
            pt->y = (*foundPointsList)[i].y;
        }
    }
    return true;
}

bool CPictogram::FastMatchTemplate(cv::Mat subImage, cv::Mat target, float corrThreshold, 
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
                                              15,
                                              1))
    {
        printf("\nERROR: Fast match template failed.\n");
        return false;
    }
    printf("number of matches = %lu\n", confidencesList->size());
    for(int i=0;i<confidencesList->size();i++)
    {
        cv::circle(source, (*foundPointsList)[i], 5, Scalar::all(255));
        printf("conf=%f\n", (*confidencesList)[i]);
    }
    return (foundPointsList->size() >= 4);
}

void CPictogram::CorrectPerspective()
{
//    if(corrected || !bbox) return;
//    
//    corrected_width = bbox[1].x - bbox[0].x;
//    corrected_height = bbox[2].y - bbox[0].y;
//    
//    CvPoint2D32f *original= (CvPoint2D32f *) cvAlloc (4 * sizeof (CvPoint2D32f));
//    original[0].x = 0;                  original[0].y = 0;
//    original[1].x = corrected_width;    original[1].y = 0;
//    original[2].x = 0;                  original[2].y = corrected_height;
//    original[3].x = corrected_width;    original[3].y = corrected_height;
//    
//    CvMat* cvR = cvCreateMat(3,3,DataType<float>::type);
//    cvGetPerspectiveTransform(original, bbox, cvR);
//    
//    IplImage* undistorted = cvCreateImage( cvSize( corrected_width, corrected_height ), 8, 3);
//    cvWarpPerspective(image, undistorted, cvR, CV_WARP_INVERSE_MAP+CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
//
//    cvReleaseImage(&image);
//    image = undistorted;
//    corrected = true;
}

U64 CPictogram::Decode()
{
    if(!corrected) return 0;
    //image_gray = cvCreateImage( cvGetSize( image ), 8, 1 );
    cvtColor( image, image_gray, CV_RGB2GRAY );
    
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
    
    for(int i=0;i<=10;i++)
    {
        for(int j=0;j<=10;j++)
        {
            Vec2f& symbol = fft2MegaPixel.at<Vec2f>(i, j);
            printf("%f+%fi\t", symbol[0], symbol[1]);
        }
        printf("\n");
    }
    
    U64 result = 0;
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
            a = a << ((i-1)*5 + j-1)*3;
            result = result | a;
        }
        
    }
    result = result & 0xffffffff; //take lower 32 bits
    printf("result = %llu\ncompare = 3402504572\n", result);
    return result;
}

U32 CPictogram::SumMegapixel(int mpx, int mpy, float megapixel_width, float megapixel_height)
{
    if(!corrected) return 0;
    U32 acc = 0;
    Mat gray(image_gray);
    
    for(int i=floor(mpx*megapixel_height);i<floor((mpx+1)*megapixel_height); i++)
    {
        for(int j=floor(mpy*megapixel_width);j<floor((mpy+1)*megapixel_width); j++)
        {
            acc+= gray.at<unsigned char>(i,j);
        }
    }
    
    return acc;
}
