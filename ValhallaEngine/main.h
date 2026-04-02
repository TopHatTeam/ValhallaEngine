// -------------------------------------------------
//
//  MIT License
//
//  main.h
//  Valhalla Engine 
//
//  Created by Andrew Skatzes on 12/31/25.
//
// -------------------------------------------------

#ifndef MAIN_H
#define MAIN_H


#if defined(__APPLE__) 
	#import <TargetConditionals.h>

#if TARGET_OS_IOS || TARGET_OS_MACCATALYST
	#import <UIKit/UIKit.h>

#elif TARGET_OS_OSX
	#import <Cocoa/Cocoa.h>

#elif defined(_WIN32) || defined(_WIN64)
	#include <Windows.h>

#elif defined(__linux__) 

#endif


#endif

#endif
