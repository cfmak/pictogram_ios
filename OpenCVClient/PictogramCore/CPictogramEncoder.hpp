#ifndef CPICTOGRAMENCODER_HPP
#define CPICTOGRAMENCODER_HPP

#include <OpenCV/opencv2/opencv.hpp>
#include <vector>
#include "PictoType.hpp"
#include "Rs.h"


class CPictogramEncoder
{
public:
    CPictogramEncoder();
    CPictogramEncoder(cv::Mat img);//, cv::Mat corner);
    ~CPictogramEncoder();
    
    cv::Mat GetOriginalImage();
    cv::Mat GetCodedImage();
    
    //cv::Mat LoadImage(const char* filepath);
    //void ShowImage();
    
    //raws is the raw number before RS and QPSK
    bool Encode(U64 raw);
    
protected:
    float maxWithinMat(cv::Mat& x);
    int SumMegapixel(cv::Mat& img, int mpx, int mpy, float megapixel_width, float megapixel_height);
    
    bool codeSuccess;
    cv::Mat preImage;
    cv::Mat postImage;
    
    cv::Mat cornerImage;
    
    float aspect;

    Rs rsencoder;
};

#endif