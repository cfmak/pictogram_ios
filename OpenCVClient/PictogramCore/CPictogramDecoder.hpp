#ifndef CPICTOGRAMDECODER_HPP
#define CPICTOGRAMDECODER_HPP

#include <OpenCV/opencv2/opencv.hpp>
#include <vector>
#include "PictoType.hpp"
#include "Rs.h"

//struct _IplImage;
//typedef _IplImage IplImage;

//struct CvMemStorage;
//class Point2f;



class CPictogramDecoder
{
public:
    enum Checker{TLChecker, TRChecker, BLChecker, BRCherker};
    CPictogramDecoder();
    CPictogramDecoder(cv::Mat img, cv::Mat tlCorner, cv::Mat trCorner, cv::Mat blCorner, cv::Mat brCorner);
    ~CPictogramDecoder();
    
    cv::Mat GetImage();
    
    //cv::Mat LoadImage(const char* filepath);
    //void ShowImage();
    
    bool FindBBoxFlann(int* ret);
    bool FindBBox(int* ret, float corrThreshold);
    bool FindNearestPoint(cv::Point ref, std::vector<cv::Point>*  foundPointsList, 
                          std::vector<double>*  confidencesList, cv::Point* pt, double* conf);
    bool FastMatchTemplate(cv::Mat subImage, cv::Mat target, float corrThreshold, 
                           std::vector<cv::Point>*  foundPointsList,
                           std::vector<double>* confidencesList);
    bool CorrectPerspective();
    
    cv::Mat GetUndistorted();
    bool Decode(unsigned int& out);
    
    //debug only
    //std::vector<cv::Point>  cornerList;
    std::vector<double> confList;
    
protected:
    U32 SumMegapixel(int mpx, int mpy, float megapixel_width, float megapixel_height);
    
    cv::Point2f bbox[4];
    
    cv::Mat eqlize;
    cv::Mat image;
    cv::Mat undistorted;
    //cv::Mat image_gray;
    
    cv::Mat tl;
    cv::Mat tr;
    cv::Mat bl;
    cv::Mat br;
    
    //IplImage* image;
    //IplImage* image_gray;
    bool corrected;
    int corrected_height;
    int corrected_width;
    
    Rs rsdecoder;
};

#endif