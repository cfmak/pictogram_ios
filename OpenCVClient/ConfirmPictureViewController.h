#import <UIKit/UIKit.h>

@interface ConfirmPictureViewController : UIViewController
{
    UIImage* image;
}

-(IBAction)confirmPictogramButton:(id)sender;
//-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender;

@property (nonatomic, retain) IBOutlet UIImageView *imageView;
@property (nonatomic, retain) UIImage *image;
@end
