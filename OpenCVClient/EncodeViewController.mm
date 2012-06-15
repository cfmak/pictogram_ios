#import "EncodeViewController.h"
#import "UIImage+OpenCV.h"

@implementation EncodeViewController

@synthesize imageView= _imageView;

#pragma mark -
#pragma mark View Controller

- (void)viewDidLoad
{   
//    if (!imagePickerController) {
//        imagePickerController = [[UIImagePickerController alloc] init];
//        imagePickerController.delegate = self;
//        
//        // If our device has a camera, we want to take a picture, otherwise, we just pick from
//        // photo library
//        if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
//        {
//            [imagePickerController setSourceType:UIImagePickerControllerSourceTypeCamera];
//        }else
//        {
//            [imagePickerController setSourceType:UIImagePickerControllerSourceTypePhotoLibrary];
//            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Attention" message:@"Your device does not support taking photos from camera. Redirecting you to Photos Library instead." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
//            [alert show];
//        }
//        
//        // image picker needs a delegate so we can respond to its messages
//        imagePickerController.delegate = self;
//    }
//    // Place image picker on the screen
//    [self presentModalViewController:imagePickerController animated:YES];
    image = nil;
    self.imageView.contentMode = UIViewContentModeScaleAspectFill;
}

-(void) viewDidAppear:(BOOL)animated
{
    NSLog(@"appeared!");
    if(imagePickerController)
    {
        [imagePickerController release];
        imagePickerController = nil;
    }
}

-(void) viewDidDisappear:(BOOL)animated
{
    NSLog(@"disappeared!");
}

- (void)viewDidUnload
{
    self.imageView = nil;
//    self.capturedImages = nil;
}

- (void)dealloc
{	
	[imageView release];
//	[capturedImages release];
    
    [super dealloc];
}

- (IBAction)takePictureButton:(id)sender
{
    if (!imagePickerController) {
        imagePickerController = [[UIImagePickerController alloc] init];
        //imagePickerController.delegate = self;
        
        // If our device has a camera, we want to take a picture, otherwise, we just pick from
        // photo library
        if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera])
        {
            [imagePickerController setSourceType:UIImagePickerControllerSourceTypeCamera];
        }
        
        // image picker needs a delegate so we can respond to its messages
        imagePickerController.delegate = self;
    }
    // Place image picker on the screen
    [self presentModalViewController:imagePickerController animated:YES];
}


- (IBAction)cameraRollButton:(id)sender
{
    if (!imagePickerController) {
        imagePickerController = [[UIImagePickerController alloc] init];
        //imagePickerController.delegate = self;
        
        // If our device has a camera, we want to take a picture, otherwise, we just pick from
        // photo library
        
        [imagePickerController setSourceType:UIImagePickerControllerSourceTypePhotoLibrary];
        
        // image picker needs a delegate so we can respond to its messages
        imagePickerController.delegate = self;
    }
    // Place image picker on the screen
    [self presentModalViewController:imagePickerController animated:YES];
}

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    NSLog(@"didFinishPickingMediaWithInfo");
    [picker dismissModalViewControllerAnimated:YES];
    //dismissed, but not released
    
    if(image)
    {
        [image release];
        image=nil;
    }
    image = [[info objectForKey:UIImagePickerControllerOriginalImage] retain];
    if(image == nil)
    {
        NSLog(@"original image nil");
    }
    else
    {
        self.imageView.image = image;
    }
}

-(IBAction)encodePictureButton:(id)sender
{
    if(image)
    {
        cv::Mat m = [image CVMat];
        CPictogramEncoder* picto = new CPictogramEncoder(m);
        if(picto->Encode(19754616))
        {
            pictogram = [UIImage imageWithCVMat: picto->GetCodedImage()];
        
            NSLog(@"call performSegueWithIdentifier");
            [self performSegueWithIdentifier:@"ConfirmPictogramSegue" sender:sender];
        }
    }
    
}

-(void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{ 
    NSLog(@"prepareForSegue");
    //UIButton *button = (UIButton *)sender; 
    //UIViewController *vc = [segue destinationViewControlEncodeMessageSegueler]; 
    //vc.title = button.titleLabel.text; 
    if ([[segue identifier] isEqualToString:@"ConfirmPictogramSegue"]) {
        
        // Get destination view
        ConfirmPictureViewController *vc = [segue destinationViewController];
        
        // Get button tag number (or do whatever you need to do here, based on your object
//        NSInteger tagIndex = [(UIButton *)sender tag];
        
        // Pass the information to your destination view
//        [vc setSelectedButton:tagIndex];
        [vc setImage:pictogram];
    }
}

//#pragma mark -
//#pragma mark Toolbar Actions
//
//- (void)showImagePicker:(UIImagePickerControllerSourceType)sourceType
//{
//    if (self.imageView.isAnimating)
//        self.imageView.stopAnimating;
//	
//    if (self.capturedImages.count > 0)
//        [self.capturedImages removeAllObjects];
//    
//    if ([UIImagePickerController isSourceTypeAvailable:sourceType])
//    {
//        [self.overlayViewController setupImagePicker:sourceType];
//        [self presentModalViewController:self.overlayViewController.imagePickerController animated:YES];
//    }
//}
//
//- (IBAction)photoLibraryAction:(id)sender
//{   
//	[self showImagePicker:UIImagePickerControllerSourceTypePhotoLibrary];
//}
//
//- (IBAction)cameraAction:(id)sender
//{
//    [self showImagePicker:UIImagePickerControllerSourceTypeCamera];
//}
//
//
//#pragma mark -
//#pragma mark OverlayViewControllerDelegate
//
// as a delegate we are being told a picture was taken
//- (void)didTakePicture:(UIImage *)picture
//{
//    NSLog(@"didTakePicture");
////    [self.capturedImages addObject:picture];
//}
//
//// as a delegate we are told to finished with the camera
//- (void)didFinishWithCamera
//{
//    [self dismissModalViewControllerAnimated:YES];
//    
//    if ([self.capturedImages count] > 0)
//    {
//        if ([self.capturedImages count] == 1)
//        {
//            // we took a single shot
//            [self.imageView setImage:[self.capturedImages objectAtIndex:0]];
//        }
//        else
//        {
//            // we took multiple shots, use the list of images for animation
//            self.imageView.animationImages = self.capturedImages;
//            
//            if (self.capturedImages.count > 0)
//                // we are done with the image list until next time
//                [self.capturedImages removeAllObjects];  
//            
//            self.imageView.animationDuration = 5.0;    // show each captured photo for 5 seconds
//            self.imageView.animationRepeatCount = 0;   // animate forever (show all photos)
//            self.imageView.startAnimating;
//        }
//    }
//}

@end