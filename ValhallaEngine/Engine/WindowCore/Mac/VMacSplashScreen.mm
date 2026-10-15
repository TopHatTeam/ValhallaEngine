// -------------------------------------------------
//
//  MIT License
//
//  VMacSplashScreen.mm
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 9/8/26.
//
// -------------------------------------------------

#include "headers/VMacSplashScreen.h"
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

using namespace ValhallaEngine::Mac;

@interface VSplashScreen : NSWindow<NSWindowDelegate>

@end

@interface VSplashView : NSView

@property (nonatomic, strong) NSImage* VSplashImage;

@end

@implementation VSplashView

- (void)drawRect:(NSRect)dirtyRect
{
    [self.VSplashImage drawInRect:self.bounds];
}

@end

@implementation VSplashScreen

- (BOOL)canBecomeMainWindow
{
    return YES;
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

@end

static VSplashScreen* SplashWindow;

void VMacSplash::DisplaySplash(const char* filepath)
{
    @autoreleasepool
    {
        NSRect SplashRect;
        
        NSString* path = [NSString stringWithUTF8String:filepath];
        NSImage* SplashImage = [[NSImage alloc] initWithContentsOfFile:path];
        
        NSBitmapImageRep* SplashBitmapRep = [NSBitmapImageRep imageRepWithData:[SplashImage TIFFRepresentation]];
        
        NSSize SplashSize = SplashImage.size;
        
        [SplashImage setSize:NSMakeSize(SplashSize.width, SplashSize.height)];
        
        SplashRect.origin.x     = 0;
        SplashRect.origin.y     = 0;
        SplashRect.size.width   = SplashSize.width;
        SplashRect.size.height  = SplashSize.height;
        
        SplashWindow = [[VSplashScreen alloc] initWithContentRect:SplashRect styleMask:NSWindowStyleMaskBorderless backing:NSBackingStoreBuffered defer:NO];
        
        VSplashView* SplashView = [[VSplashView alloc] initWithFrame:SplashRect];
        
        SplashView.VSplashImage = SplashImage;
        
        [SplashWindow setContentView:SplashView];
        [SplashWindow center];
        [SplashWindow makeKeyAndOrderFront:nil];
    }
}

void VMacSplash::Desconstruct()
{
    // We're technically doing the same as 'free(SplashWindow)'
    // Objective-C has a dealloc function but I don't really understand it
    // So, this method would be the same
    SplashWindow = nil;
    
}
