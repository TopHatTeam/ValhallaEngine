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
    NSRect WindowContentSize = NSMakeRect(0, 0, 1920, 1080);
    
    NSUInteger WindowStyleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;
    
    self.window = [[NSWindow alloc] initWithContentRect:WindowContentSize styleMask:WindowStyleMask backing:NSBackingStoreBuffered defer:NO];
    
    [self.window setTitle:@"Valhalla Engine Editor"];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [self.window setDelegate:self];
    
    // View layout
    //{
    //    ObjectView* ObjViewer = [[ObjectView alloc] initWithFrame:NSMakeRect(0, 0, 200, 720)];
    //    [self.window.contentView addSubview:ObjViewer];
    //}
    
    NSTabView* EditorTabs = [[NSTabView alloc] initWithFrame:NSMakeRect(0, 0, 1000, 400)];
    NSTabViewItem* AssetsTab = [[NSTabViewItem alloc] init];
    [AssetsTab setLabel:@"Assets"];
    
    EditorTabs.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    
    NSTabViewItem* ClassesTab = [[NSTabViewItem alloc] init];
    [ClassesTab setLabel:@"Classes"];
    
    NSView* FirstTabView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 1000, 400)];
    [FirstTabView setWantsLayer:YES];
    [FirstTabView.layer setBackgroundColor:[[NSColor grayColor] CGColor]];
    [AssetsTab setView:FirstTabView];
    
    [EditorTabs addTabViewItem:AssetsTab];
    [EditorTabs addTabViewItem:ClassesTab];
    
    [self.window.contentView addSubview:EditorTabs];
    
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

- (void) showAboutPanel:(id)sender
{
    // About information
    NSDictionary* options =
    @{
        @"ApplicationName": @"Valhalla Engine Editor",
        @"Version": @"1.0",
        @"Copyright": @"© 2025-2026 Andrew Skatzes"
    };
    [NSApp orderFrontStandardAboutPanelWithOptions:options];
}

@end

int main(int argc, const char * argv[])
{
    @autoreleasepool
    {
        // Setup code that might create autoreleased objects goes here.
        NSApplication* app = [NSApplication sharedApplication];
        
        AppDelegate* appDelegate = [[AppDelegate alloc] init];
        [app setDelegate:appDelegate];
        
        NSMenu* MainMenu = [[NSMenu alloc] init];
        [NSApp setMainMenu:MainMenu];
        
        NSMenuItem* ValhallaMenuItem = [[NSMenuItem alloc] init];
        
        [MainMenu addItem:ValhallaMenuItem];
        
        NSMenu* ValhallaMenu = [[NSMenu alloc] initWithTitle:@"Valhalla Engine Editor"];
        
        [ValhallaMenuItem setSubmenu:ValhallaMenu];
        
        NSString* ApplicationName = [[NSProcessInfo processInfo] processName];
        
        NSString* AboutTitle = [@"About " stringByAppendingString:ApplicationName];
        NSMenuItem* AboutItem = [[NSMenuItem alloc] initWithTitle:AboutTitle action:@selector(showAboutPanel:) keyEquivalent:@""];
        [AboutItem setTarget:appDelegate];
        
        [ValhallaMenu addItemWithTitle:[@"About " stringByAppendingString:ApplicationName] action:@selector(showAboutPanel:) keyEquivalent:@""];
        
        [ValhallaMenu addItem:[NSMenuItem separatorItem]];
        
        [ValhallaMenu addItemWithTitle:[@"Quit " stringByAppendingString:ApplicationName]action:@selector(terminate:) keyEquivalent:@"q"];
        
        [app run];
    }
    return 0;
}

