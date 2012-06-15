//
//  OpenCVClientViewController.m
//  OpenCVClient
//
//  Created by Robin Summerhill on 02/09/2011.
//  Copyright 2011 Aptogo Limited. All rights reserved.
//
//  Permission is given to use this source code file without charge in any
//  project, commercial or otherwise, entirely at your risk, with the condition
//  that any redistribution (in part or whole) of source code must retain
//  this copyright and permission notice. Attribution in compiled projects is
//  appreciated but not required.
//

// UIImage extensions for converting between UIImage and cv::Mat
#import "UIImage+OpenCV.h"
#include "CPictogramDecoder.hpp"
#import "ScanViewController.h"

// Aperture value to use for the Canny edge detection
const int kCannyAperture = 7;

@interface ScanViewController ()
- (void)processFrame;
@end

@implementation ScanViewController
@synthesize imageView = _imageView;
@synthesize elapsedTimeLabel = _elapsedTimeLabel;
@synthesize tlfound = _tlfound;
@synthesize trfound = _trfound;
@synthesize blfound = _blfound;
@synthesize brfound = _brfound;
@synthesize decodedLabel = _decodedLabel;


- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image {
    // Getting CGImage from UIImage
    CGImageRef imageRef = image.CGImage;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    // Creating temporal IplImage for drawing
    IplImage *iplimage = cvCreateImage(
                                       cvSize(image.size.width,image.size.height), IPL_DEPTH_8U, 4
                                       );
    // Creating CGContext for temporal IplImage
    CGContextRef contextRef = CGBitmapContextCreate(
                                                    iplimage->imageData, iplimage->width, iplimage->height,
                                                    iplimage->depth, iplimage->widthStep,
                                                    colorSpace, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault
                                                    );
    // Drawing CGImage to CGContext
    CGContextDrawImage(
                       contextRef,
                       CGRectMake(0, 0, image.size.width, image.size.height),
                       imageRef
                       );
    CGContextRelease(contextRef);
    CGColorSpaceRelease(colorSpace);
    
    // Creating result IplImage
    IplImage *ret = cvCreateImage(cvGetSize(iplimage), IPL_DEPTH_8U, 3);
    cvCvtColor(iplimage, ret, CV_RGBA2BGR);
    cvReleaseImage(&iplimage);
    
    return ret;
}

// NOTE You should convert color mode as RGB before passing to this function
- (UIImage *)UIImageFromIplImage:(IplImage *)image {
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    // Allocating the buffer for CGImage
    NSData *data =
    [NSData dataWithBytes:image->imageData length:image->imageSize];
    CGDataProviderRef provider =
    CGDataProviderCreateWithCFData((CFDataRef)data);
    // Creating CGImage from chunk of IplImage
    CGImageRef imageRef = CGImageCreate(
                                        image->width, image->height,
                                        image->depth, image->depth * image->nChannels, image->widthStep,
                                        colorSpace, kCGImageAlphaNone|kCGBitmapByteOrderDefault,
                                        provider, NULL, false, kCGRenderingIntentDefault
                                        );
    // Getting UIImage from CGImage
    UIImage *ret = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    return ret;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
       
//    UIImage *tl = [UIImage imageNamed:@"checkerboardCorner12.bmp"];
//    UIImage *tr = [UIImage imageNamed:@"checkerboardCorner12.bmp"];
//    UIImage *bl = [UIImage imageNamed:@"checkerboardCorner12.bmp"];
//    UIImage *br = [UIImage imageNamed:@"checkerboardCorner12.bmp"];
    UIImage *tl = [UIImage imageNamed:@"crossCorner12.jpg"];
    UIImage *tr = [UIImage imageNamed:@"crossCorner12.jpg"];
    UIImage *bl = [UIImage imageNamed:@"crossCorner12.jpg"];
    UIImage *br = [UIImage imageNamed:@"crossCorner12.jpg"];
    
    tlcorner = [tl CVMat];
    cvtColor( tlcorner, tlcorner, CV_RGB2GRAY );
    trcorner = [tr CVMat];
    cvtColor( trcorner, trcorner, CV_RGB2GRAY );
    blcorner = [bl CVMat];
    cvtColor( blcorner, blcorner, CV_RGB2GRAY );
    brcorner = [br CVMat];
    cvtColor( brcorner, brcorner, CV_RGB2GRAY );
    
    found = false;
    
    self.imageView.image = [UIImage imageWithCVMat:trcorner];
    
    //[self scheduleDetectTimer];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self scheduleDetectTimer];
}

-(void) viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [self stopDetectTimer];
}

-(bool) openVideoCapture
{
#if TARGET_IPHONE_SIMULATOR
    NSLog(@"Video capture is not supported in the simulator");
    return NO;
#else
    if(_videoCapture)
        [self closeVideoCapture];
    _videoCapture = new cv::VideoCapture;
    if (_videoCapture && !_videoCapture->open(CV_CAP_AVFOUNDATION))
    {
        NSLog(@"Failed to open video camera");
        return NO;
    }
    return YES;
#endif
}

-(void) closeVideoCapture
{
    if(_videoCapture && _videoCapture->isOpened())
    {
        _videoCapture->release();
        delete _videoCapture;
        _videoCapture = nil;
    }
}

-(void) stopDetectTimer
{
    if(timer)
    {
        [timer invalidate];
        [timer release];
        timer = nil;
        [self closeVideoCapture];
    }
}

-(void) scheduleDetectTimer
{
    if(!timer)
    {
        // Initialise video capture - only supported on iOS device NOT simulator
#if TARGET_IPHONE_SIMULATOR
        NSLog(@"Video capture is not supported in the simulator");
#else
        if(!_videoCapture)
            _videoCapture = new cv::VideoCapture;
        if (_videoCapture && !_videoCapture->open(CV_CAP_AVFOUNDATION))
        {
            NSLog(@"Failed to open video camera");
        }
#endif
        timer = [[NSTimer scheduledTimerWithTimeInterval:0.1f
                                                       target:self
                                                     selector:@selector( detectLoop )
                                                     userInfo:nil
                                                      repeats:YES]
                    retain];
       
    }
}


- (void)viewDidUnload
{
    [super viewDidUnload];
    [self stopDetectTimer];
    self.imageView = nil;
    self.elapsedTimeLabel = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

// Called when the user taps the Capture button. Grab a frame and process it
- (IBAction)reset:(id)sender
{
    found = false;
}

-(void)detectLoop
{
    //NSLog(@"detectLoop\n");
    if(!_videoCapture || !_videoCapture->isOpened())
    {
        [self openVideoCapture];
        //do detect in the next loop
        return;
    }
    
    if (_videoCapture && _videoCapture->isOpened() && _videoCapture->grab())
    {
        cv::Mat bgrFrame;
        if(_videoCapture && _videoCapture->read(bgrFrame))
        {
            cvtColor(bgrFrame, _lastFrame, CV_BGR2GRAY);
        //NSLog(@"dimension = %d %d\n", _lastFrame.size().width, _lastFrame.size().height); //360*480
            [self processFrame];
        }
    }
    else
    {
        NSLog(@"Failed to grab frame");
        [self stopDetectTimer];
    }
}

// Perform image processing on the last captured frame and display the results
- (void)processFrame
{
    if(found)
        return;
    
    double t = (double)cv::getTickCount();
    //pyrUp(_lastFrame, _lastFrame);
    
    cv::Mat roi = _lastFrame(cv::Rect(0,_lastFrame.size().height - _lastFrame.size().width,_lastFrame.size().width, _lastFrame.size().width));
//    printf("width = %d", _lastFrame.size().width);
    CPictogramDecoder* picto = new CPictogramDecoder(roi, tlcorner, trcorner, blcorner, brcorner);
    
    int ret = 0;
    if(picto->FindBBox(&ret, 70) == true)
    {
        self.tlfound.text = [NSString stringWithFormat:@"%lf", picto->confList[0]];
        self.trfound.text = [NSString stringWithFormat:@"%lf", picto->confList[1]];
        self.blfound.text = [NSString stringWithFormat:@"%lf", picto->confList[2]];
        self.brfound.text = [NSString stringWithFormat:@"%lf", picto->confList[3]];
        if(picto->CorrectPerspective())
        {
            found = true;
            self.imageView.image = [UIImage imageWithCVMat: picto->GetUndistorted()];
            U32 code = 0;
            if(picto->Decode(code))
            {
                self.decodedLabel.text = [NSString stringWithFormat:@"%x", code];
            }
            else
            {
                self.decodedLabel.text = [NSString stringWithFormat:@"decode fail"];
            }
        }
    }
    else
    {
        self.tlfound.text = @"0";
        self.trfound.text = @"0";
        self.blfound.text = @"0";
        self.brfound.text = @"0";
    }
    
    t = 1000 * ((double)cv::getTickCount() - t) / cv::getTickFrequency();
    
    // Display result
//    self.tlfound.text = [NSString stringWithFormat:@"%d", (ret&1)>0];
//    self.trfound.text = [NSString stringWithFormat:@"%d", (ret&2)>0];
//    self.blfound.text = [NSString stringWithFormat:@"%d", (ret&4)>0];
//    self.brfound.text = [NSString stringWithFormat:@"%d", (ret&8)>0];
    
    //self.imageView.image = [UIImage imageWithCVMat: _lastFrame];
    if(!found)
    {
        self.imageView.image = [UIImage imageWithCVMat: picto->GetImage()];
        self.elapsedTimeLabel.text = [NSString stringWithFormat:@"%.1fms", t];
    }
}

@end
