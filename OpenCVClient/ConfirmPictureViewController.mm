#import "ConfirmPictureViewController.h"

@implementation ConfirmPictureViewController

@synthesize imageView= _imageView;
@synthesize image;

- (void)viewDidLoad
{
    self.imageView.image=image;
    self.imageView.contentMode = UIViewContentModeScaleAspectFit;
}

-(IBAction)confirmPictogramButton:(id)sender
{
    [self performSegueWithIdentifier:@"EncodeMessageSegue" sender:sender];
    
}

-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{ 
    NSLog(@"prepareForSegue");
    //UIButton *button = (UIButton *)sender; 
    //UIViewController *vc = [segue destinationViewControlEncodeMessageSegueler]; 
    //vc.title = button.titleLabel.text; 
    if ([[segue identifier] isEqualToString:@"EncodeMessageSegue"]) {
        
        // Get destination view
        UIViewController *vc = [segue destinationViewController];
        
        // Get button tag number (or do whatever you need to do here, based on your object
        //        NSInteger tagIndex = [(UIButton *)sender tag];
        
        // Pass the information to your destination view
        //        [vc setSelectedButton:tagIndex];
        //[vc setImage:pictogram];
    }
}

@end