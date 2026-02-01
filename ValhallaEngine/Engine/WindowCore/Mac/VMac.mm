// -------------------------------------------------
//
//  MIT License
//
//  v_mac.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/22/26.
//
// -------------------------------------------------

#include "headers/VMac.h"
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

// yes this code was written on a mac

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

VMacWindow::VMacWindow(uint32 WinWidth, uint32 WinHeight, bool WinVisible, bool WinActive)
{
    WindowWidth         = WinWidth;
    WindowHeight        = WinHeight;
    bIsWindowVisible    = WinVisible;
    bIsActiveWindow     = WinActive;
}

VMacWindow::~VMacWindow()
{
    WindowWidth         = 0;
    WindowHeight        = 0;
    bIsWindowVisible    = false;
    bIsActiveWindow     = false;
}

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
