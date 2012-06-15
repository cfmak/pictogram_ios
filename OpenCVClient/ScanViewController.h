//
//  OpenCVClientViewController.h
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

#import <UIKit/UIKit.h>

@interface ScanViewController : UIViewController
{
    cv::VideoCapture *_videoCapture;
    cv::Mat _lastFrame;
    
    cv::Mat tlcorner;
    cv::Mat trcorner;
    cv::Mat blcorner;
    cv::Mat brcorner;
    
    bool found;
    
    NSTimer *timer;
}

@property (nonatomic, retain) IBOutlet UIImageView *imageView;
@property (nonatomic, retain) IBOutlet UILabel *elapsedTimeLabel;

@property (nonatomic, retain) IBOutlet UILabel *tlfound;
@property (nonatomic, retain) IBOutlet UILabel *trfound;
@property (nonatomic, retain) IBOutlet UILabel *blfound;
@property (nonatomic, retain) IBOutlet UILabel *brfound;
@property (nonatomic, retain) IBOutlet UILabel *decodedLabel;


- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image;
- (UIImage *)UIImageFromIplImage:(IplImage *)image;
- (IBAction)reset:(id)sender;
-(void) scheduleDetectTimer;
-(void) stopDetectTimer;
-(void)detectLoop;

-(bool) openVideoCapture;
-(void) closeVideoCapture;

@end
