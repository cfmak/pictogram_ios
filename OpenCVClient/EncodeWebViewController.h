#import <UIKit/UIKit.h>

@interface EncodeWebViewController : UIViewController {
    IBOutlet UIWebView *webView;
}

-(IBAction)ConfirmTextButton:(id)sender;

@property (nonatomic, retain) UIWebView *webView;

@end
