// -------------------------------------------------
//
//  MIT License
//
//  main.mm
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 12/31/25.
//
// -------------------------------------------------

#import "main.h"
//#include "v_datapackage.hpp"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
    NSRect WindowContentSize = NSMakeRect(0, 0, 1280, 720);
    
    NSUInteger WindowStyleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;
    
    self.window = [[NSWindow alloc] initWithContentRect:WindowContentSize styleMask:WindowStyleMask backing:NSBackingStoreBuffered defer:NO];
    
    [self.window setTitle:@"Valhalla Engine Editor"];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [self.window setDelegate:self];
    
    [NSApp activateIgnoringOtherApps:YES];
}

-(BOOL)windowShouldClose:(NSWindow*)sender
{
    [NSApp terminate:nil];
    return YES;
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

@end

int main(int argc, const char * argv[])
{
    @autoreleasepool
    {
        // Setup code that might create autoreleased objects goes here.
        NSApplication* app = [NSApplication sharedApplication];
        
        NSMenu* MainMenu = [[NSMenu alloc] init];
        [NSApp setMainMenu:MainMenu];
        
        NSMenuItem* ValhallaMenuItem = [[NSMenuItem alloc] init];
        
        [MainMenu addItem:ValhallaMenuItem];
        
        NSMenu* ValhallaMenu = [[NSMenu alloc] initWithTitle:@"Valhalla Engine Editor"];
        
        [ValhallaMenuItem setSubmenu:ValhallaMenu];
        
        NSString* ApplicationName = [[NSProcessInfo processInfo] processName];
        
        [ValhallaMenu addItemWithTitle:[@"About " stringByAppendingString:ApplicationName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

        [ValhallaMenu addItem:[NSMenuItem separatorItem]];
        
        [ValhallaMenu addItemWithTitle:[@"Quit " stringByAppendingString:ApplicationName]action:@selector(terminate:) keyEquivalent:@"q"];
        
        AppDelegate* appDelegate = [[AppDelegate alloc] init];
        [app setDelegate:appDelegate];
        
        [app run];
    }
    return 0;
}
