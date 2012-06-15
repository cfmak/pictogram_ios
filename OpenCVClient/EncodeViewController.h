#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioServices.h>

#include "CPictogramEncoder.hpp"
#import "ConfirmPictureViewController.h"

@interface EncodeViewController : UIViewController <UIImagePickerControllerDelegate,
UINavigationControllerDelegate>
{
    UIImage* image;
    UIImageView *imageView;
    UIImagePickerController* imagePickerController;
    UIImage* pictogram;
    
    //OverlayViewController *overlayViewController; // the camera custom overlay view
    
//    NSMutableArray *capturedImages; // the list of images captures from the camera (either 1 or multiple)
}

@property (nonatomic, retain) IBOutlet UIImageView *imageView;
//@property (nonatomic, retain) NSMutableArray *capturedImages;

// toolbar buttons
//- (IBAction)photoLibraryAction:(id)sender;
//- (IBAction)cameraAction:(id)sender;

- (IBAction)takePictureButton:(id)sender;
- (IBAction)cameraRollButton:(id)sender;
-(IBAction)encodePictureButton:(id)sender;
@end

