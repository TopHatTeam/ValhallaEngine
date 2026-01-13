// -------------------------------------------------
//
//  MIT License
//
//  main.h
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 12/31/25.
//
// -------------------------------------------------

#ifndef MAIN_H
#define MAIN_H

#import <TargetConditionals.h>

#if TARGET_OS_IOS || TARGET_OS_MACCATALYST
#import <UIKit/UIKit.h>
#elif TARGET_OS_OSX
#import <Cocoa/Cocoa.h>
#else

#endif

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@property (nonatomic, strong) NSWindow* window;
@end

#endif
