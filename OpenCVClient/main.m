//
//  main.m
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

#import "OpenCVClientAppDelegate.h"

int main(int argc, char *argv[])
{
//    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
//    int retVal = UIApplicationMain(argc, argv, nil, nil);
//    [pool release];
//    return retVal;
    @autoreleasepool {
		return UIApplicationMain(argc, argv, nil, 
                                 NSStringFromClass([OpenCVClientAppDelegate class]));
    }
}
