//
//  MacroKit.h
//  MacroKit
//
//  Created by Alberto García Hierro on 11/09/13.
//  Copyright (c) 2013 Rainy Cape S.L. All rights reserved.
//

#import <Foundation/Foundation.h>

// Macros for both OS X and iOS

// Declaring a singleton using GCD, if available, or synchronized(self) otherwise
#define SINGLETON_FOR_CLASS_DECL_NAME(cls, name)	+ (cls *)shared##name
#define SINGLETON_FOR_CLASS_DECL(cls)   SINGLETON_FOR_CLASS_DECL_NAME(cls, cls)

// Check for blocks availability at runtime
#define ARE_BLOCKS_AVAILABLE() (NSClassFromString(@"NSBlockOperation") != nil)

#define __macro_kit_initialize_singleton_synchronized(var)  \
    if (!var) { \
        @synchronized(self) { \
            if (!var) { \
                var = [[super allocWithZone:NULL] init]; \
            } \
        } \
    }

#if NS_BLOCKS_AVAILABLE
#define __macro_kit_initialize_singleton(var)    \
    if (ARE_BLOCKS_AVAILABLE()) { \
        static dispatch_once_t once; \
        dispatch_once(&once, ^{ \
            _shared##name = [[super allocWithZone:NULL] init]; \
        }); \
    } else { \
        __macro_kit_initialize_singleton_synchronized(var); \
    }
#else
#define __macro_kit_initialize_singleton(var) __macro_kit_initialize_singleton_synchronized(var)
#endif

#define SINGLETON_FOR_CLASS_NAME(cls, name) \
    static cls *_shared##name; \
    - (oneway void)release { \
    } \
    - (id)retain { \
        return self; \
    } \
    - (id)autorelease { \
        return self; \
    } \
    - (id)copyWithZone:(NSZone *)zone { \
        return self; \
    } \
    - (NSUInteger)retainCount { \
        return UINT_MAX; \
    } \
    + (id)allocWithZone:(NSZone *)zone { \
        return [self shared##name]; \
    } \
    + (cls *)shared##name { \
        __macro_kit_initialize_singleton(_shared##name); \
        return _shared##name; \
    }

#define SINGLETON_FOR_CLASS(cls)    SINGLETON_FOR_CLASS_NAME(cls, cls)

// iOS specific macros
#if TARGET_OS_IPHONE

// Checking for device idiom
#define IS_PHONE()  (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
#define IS_PAD()    (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)

// Checking for iOS version
#define IOS_VERSION_EQ(v)   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define IOS_VERSION_GT(v)   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define IOS_VERSION_GTE(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define IOS_VERSION_LT(v)   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define IOS_VERSION_LTE(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)

#endif