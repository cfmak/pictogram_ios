#import "WebCore.h"
#import "ASIHTTPRequest.h"
#import "ASIFormDataRequest.h"

@implementation WebCore

static NSString* username;
static NSString* userpassword;

+(int) login
{
    NSLog(@"Webcore::login");
//    NSMutableURLRequest *theRequest = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:@"http://98.207.1.60/checklogin.php"]];
//    [theRequest setHTTPMethod:@"POST"];
    //[theRequest addValue:@"text/xml" forHTTPHeaderField:@"content-type"];
    //[theRequest setCachePolicy:NSURLCacheStorageNotAllowed];
    //[theRequest setTimeoutInterval:5.0];
//    NSString* email = @"myemail@gmail.com";
//    NSString* password = @"mypassword";
//    
//    NSString* pStr = [NSString stringWithFormat:@"email=%@&password=%@",
//                      [email stringByAddingPercentEscapesUsingEncoding:NSUnicodeStringEncoding],
//                      [password stringByAddingPercentEscapesUsingEncoding:NSUnicodeStringEncoding]]; 
//    NSData* pBody = [pStr dataUsingEncoding:NSUTF8StringEncoding];
//    
//    [theRequest setHTTPBody:pBody];
    username = @"myemail@gmail.com";
    userpassword = @"mypassword";
    
    NSURL *url = [NSURL URLWithString:@"http://98.207.1.60/test.php"];
    ASIFormDataRequest *request = [ASIFormDataRequest requestWithURL:url];
    
    [request addPostValue:username forKey:@"email"];
    [request addPostValue:userpassword forKey:@"password"];
    [request startSynchronous];
    NSError *error = [request error];
    if (!error) {
        NSString *response = [request responseString];
        NSLog(@"%@",response);
    }
    else
    {
        NSLog(@"connect failed");
    }
    
    //[webView loadRequest:theRequest];
}

@end