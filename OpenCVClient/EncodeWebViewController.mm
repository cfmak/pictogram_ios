#import "EncodeWebViewController.h"
#import "WebCore.h"

@implementation EncodeWebViewController

@synthesize webView;

- (void)viewDidLoad {
//    NSMutableURLRequest req = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"http://98.207.1.60/"] ];
    
//    NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"http://98.207.1.60/checklogin.php"]];
//    [theRequest setHTTPMethod:@"POST"];
//    //[theRequest addValue:@"text/xml" forHTTPHeaderField:@"content-type"];
//    //[theRequest setCachePolicy:NSURLCacheStorageNotAllowed];
//    //[theRequest setTimeoutInterval:5.0];
//    NSString* email = @"myemail@gmail.com";
//    NSString* password = @"mypassword";
//    
//    NSString* pStr = [NSString stringWithFormat:@"email=%@&password=%@",
//                       [email stringByAddingPercentEscapesUsingEncoding:NSUnicodeStringEncoding],
//                       [password stringByAddingPercentEscapesUsingEncoding:NSUnicodeStringEncoding]]; 
//    NSData* pBody = [pStr dataUsingEncoding:NSUTF8StringEncoding];
//    
//    [theRequest setHTTPBody:pBody];
//    
//    [webView loadRequest:theRequest];
    //[pBody release];
    //[pStr release];

    [WebCore login];
    
    [super viewDidLoad];
}

-(IBAction)ConfirmTextButton:(id)sender
{
    
}

@end 