#ifndef CPICTOGRAM_HPP
#define CPICTOGRAM_HPP

#include <OpenCV/opencv2/opencv.hpp>
#include <vector>

//struct _IplImage;
//typedef _IplImage IplImage;

//struct CvMemStorage;
//class Point2f;

typedef unsigned long long int U64;
typedef unsigned int U32;

class CPictogram
{
public:
    enum Checker{TLChecker, TRChecker, BLChecker, BRCherker};
    CPictogram();
    CPictogram(cv::Mat img, cv::Mat tlCorner, cv::Mat trCorner, cv::Mat blCorner, cv::Mat brCorner);
    ~CPictogram();
    
    //cv::Mat LoadImage(const char* filepath);
    //void ShowImage();
    
    bool FindBBox(int* ret, float corrThreshold);
    bool FindNearestPoint(cv::Point ref, std::vector<cv::Point>*  foundPointsList, cv::Point* pt);
    bool FastMatchTemplate(cv::Mat subImage, cv::Mat target, float corrThreshold, 
                           std::vector<cv::Point>*  foundPointsList,
                           std::vector<double>* confidencesList);
    void CorrectPerspective();
    U64 Decode();
    
protected:
    U32 SumMegapixel(int mpx, int mpy, float megapixel_width, float megapixel_height);
    
    cv::Point2f bbox[4];
    
    cv::Mat image;
    cv::Mat image_gray;
    
    cv::Mat tl;
    cv::Mat tr;
    cv::Mat bl;
    cv::Mat br;
    
    //IplImage* image;
    //IplImage* image_gray;
    bool corrected;
    int corrected_height;
    int corrected_width;
};

#endif