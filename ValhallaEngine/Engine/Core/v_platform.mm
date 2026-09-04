// -------------------------------------------------
//
//  MIT License
//
//  v_platform.mm
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/8/26.
//
// -------------------------------------------------

#import "headers/v_platform.h"
#import <Cocoa/Cocoa.h>


namespace ValhallaEngine::Platform
{
    void InfoMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        @autoreleasepool
        {
            char buffer[1024];                                  // <--- Local buffer to hold our formatted string
            
            va_list args;                                       // <--- Declare a va_list to hold the variable arguments
            va_start(args, Message);                            // <--- Initalize 'args' to point to the first argument after 'Message'
            vsnprintf(buffer, sizeof(buffer), Message, args);   // <--- Write formatted string into buffer
            va_end(args);                                       // <--- Clean up the variable argument list
            
            NSAlert* alert = [[NSAlert alloc] init];
            [alert setMessageText:[NSString stringWithUTF8String:BoxTitle]];
            [alert setInformativeText:[NSString stringWithUTF8String:buffer]];
            [alert addButtonWithTitle:@"OK"];
            [alert setAlertStyle:NSAlertStyleInformational];
            [alert runModal];
        }
    }
    
    void ErrorMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        @autoreleasepool
        {
            char buffer[1024];                                  // <--- Local buffer to hold our formatted string
            
            va_list args;                                       // <--- Declare a va_list to hold the variable arguments
            va_start(args, Message);                            // <--- Initalize 'args' to point to the first argument after 'Message'
            vsnprintf(buffer, sizeof(buffer), Message, args);   // <--- Write formatted string into buffer
            va_end(args);                                       // <--- Clean up the variable argument list
            
            NSAlert* alert = [[NSAlert alloc] init];
            [alert setMessageText:[NSString stringWithUTF8String:BoxTitle]];
            [alert setInformativeText:[NSString stringWithUTF8String:buffer]];
            [alert addButtonWithTitle:@"OK"];
            [alert setAlertStyle:NSAlertStyleWarning]; // I read the docs to get this fucker functional
            [alert runModal];
            
        }
    }
    
    void FatalMessageBox(const char* BoxTitle, const char* Message, ...)
    {
        @autoreleasepool
        {
            char buffer[1024];                                  // <--- Local buffer to hold our formatted string
            
            va_list args;                                       // <--- Declare a va_list to hold the variable arguments
            va_start(args, Message);                            // <--- Initalize 'args' to point to the first argument after 'Message'
            vsnprintf(buffer, sizeof(buffer), Message, args);   // <--- Write formatted string into buffer
            va_end(args);                                       // <--- Clean up the variable argument list
            
            NSAlert* alert = [[NSAlert alloc] init];
            [alert setMessageText:[NSString stringWithUTF8String:BoxTitle]];
            [alert setInformativeText:[NSString stringWithUTF8String:buffer]];
            [alert addButtonWithTitle:@"OK"];
            [alert setAlertStyle:NSAlertStyleCritical];
            [alert runModal];
        }
    }
}

