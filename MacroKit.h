//
//  MacroKit.h
//  MacroKit
//
//  Created by Alberto García Hierro on 11/09/13.
//  Copyright (c) 2013 Rainy Cape S.L. All rights reserved.
//

#import <Foundation/Foundation.h>

// Macros for both OS X and iOS

// Declare a singleton for the given class.
// SINGLETON_FOR_CLASS_DECL() and SINGLETON_FOR_CLASS_DECL_NAME() should be
// used in the header file for the singleton. Take a look at SINGLETON_FOR_CLASS()
// and SINGLETON_FOR_CLASS_NAME() for the implementation.
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
            var = [[super allocWithZone:NULL] init]; \
        }); \
    } else { \
        __macro_kit_initialize_singleton_synchronized(var); \
    }
#else
#define __macro_kit_initialize_singleton(var) __macro_kit_initialize_singleton_synchronized(var)
#endif

// Implement a singleton using GCD, if available, or synchronized(self) otherwise.
// SINGLETON_FOR_CLASS() and SINGLETON_FOR_CLASS_NAME() should be
// used in the implementation (.m) file for the singleton.
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

// Debugging
#ifndef DEBUG_LOG
#ifdef DEBUG
#define DEBUG_LOG(...) NSLog(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif
#endif

// From https://gist.github.com/DamnedFacts/5050238
#ifdef DEBUG
#pragma once

//based off of http://www.dribin.org/dave/blog/archives/2008/09/22/convert_to_nsstring/
static NSString * VTPG_DDToStringFromTypeAndValue(const char * typeCode, void * value);


#define DEBUG_LOG_EXPR(_X_) do{\
    __typeof__(_X_) _Y_ = (_X_); \
    const char * _TYPE_CODE_ = @encode(__typeof__(_X_)); \
    NSString *_STR_ = VTPG_DDToStringFromTypeAndValue(_TYPE_CODE_, &_Y_);\
    if(_STR_) { \
        NSLog(@"%s = %@", #_X_, _STR_); \
    } else {\
        NSLog(@"Unknown _TYPE_CODE_: %s for expression %s in function %s, file %s, line %d", \
            _TYPE_CODE_, #_X_, __func__, __FILE__, __LINE__); \
    } \
}while(0)

#define DEBUG_LOG_FUNCTION()	NSLog(@"%s", __func__)


// http://www.wilshipley.com/blog/2005/10/pimp-my-code-interlude-free-code.html
static inline BOOL IsEmpty(id thing) {
	return thing == nil ||
    ([thing respondsToSelector:@selector(length)] && [(NSData *)thing length] == 0) ||
    ([thing respondsToSelector:@selector(count)]  && [(NSArray *)thing count] == 0);
}

//based off http://www.dribin.org/dave/blog/archives/2008/09/22/convert_to_nsstring/
//
static BOOL TypeCodeIsCharArray(const char *typeCode){
    size_t len = strlen(typeCode);
    if(len <= 2)
        return NO;
    size_t lastCharOffset = len - 1;
	size_t secondToLastCharOffset = lastCharOffset - 1 ;

	BOOL isCharArray = typeCode[0] == '[' &&
    typeCode[secondToLastCharOffset] == 'c' && typeCode[lastCharOffset] == ']';
	for(int i = 1; i < secondToLastCharOffset; i++)
		isCharArray = isCharArray && isdigit(typeCode[i]);
	return isCharArray;
}

//since BOOL is #defined as a signed char, we treat the value as
//a BOOL if it is exactly YES or NO, and a char otherwise.
static NSString* VTPGStringFromBoolOrCharValue(BOOL boolOrCharvalue) {
	if(boolOrCharvalue == YES)
		return @"YES";
	if(boolOrCharvalue == NO)
		return @"NO";
	return [NSString stringWithFormat:@"'%c'", boolOrCharvalue];
}

static NSString *VTPGStringFromFourCharCodeOrUnsignedInt32(FourCharCode fourcc) {
	return [NSString stringWithFormat:@"%u ('%c%c%c%c')",
            (unsigned )fourcc,
            (int)((fourcc >> 24) & 0xFF),
            (int)((fourcc >> 16) & 0xFF),
            (int)((fourcc >> 8) & 0xFF),
            (int)(fourcc & 0xFF)];
}

static NSString *StringFromNSDecimalWithCurrentLocal(NSDecimal dcm) {
	return NSDecimalString(&dcm, [NSLocale currentLocale]);
}

static NSString * VTPG_DDToStringFromTypeAndValue(const char * typeCode, void * value) {
#define IF_TYPE_MATCHES_INTERPRET_WITH(typeToMatch,func) \
    if (strcmp(typeCode, @encode(typeToMatch)) == 0) \
        return (func)(*(typeToMatch*)value)

#if	TARGET_OS_IPHONE
	IF_TYPE_MATCHES_INTERPRET_WITH(CGPoint,NSStringFromCGPoint);
	IF_TYPE_MATCHES_INTERPRET_WITH(CGSize,NSStringFromCGSize);
	IF_TYPE_MATCHES_INTERPRET_WITH(CGRect,NSStringFromCGRect);
    IF_TYPE_MATCHES_INTERPRET_WITH(UIEdgeInsets, NSStringFromUIEdgeInsets);
#else
	IF_TYPE_MATCHES_INTERPRET_WITH(NSPoint, NSStringFromPoint);
	IF_TYPE_MATCHES_INTERPRET_WITH(NSSize, NSStringFromSize);
	IF_TYPE_MATCHES_INTERPRET_WITH(NSRect, NSStringFromRect);
#endif
	IF_TYPE_MATCHES_INTERPRET_WITH(NSRange, NSStringFromRange);
	IF_TYPE_MATCHES_INTERPRET_WITH(Class, NSStringFromClass);
	IF_TYPE_MATCHES_INTERPRET_WITH(SEL, NSStringFromSelector);
	IF_TYPE_MATCHES_INTERPRET_WITH(BOOL, VTPGStringFromBoolOrCharValue);
	IF_TYPE_MATCHES_INTERPRET_WITH(NSDecimal,StringFromNSDecimalWithCurrentLocal);

#define IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(typeToMatch,formatString) \
    if (strcmp(typeCode, @encode(typeToMatch)) == 0) \
        return [NSString stringWithFormat:(formatString), (*(typeToMatch*)value)]


	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(CFStringRef,@"%@"); //CFStringRef is toll-free bridged to NSString*
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(CFArrayRef,@"%@"); //CFArrayRef is toll-free bridged to NSArray*
	IF_TYPE_MATCHES_INTERPRET_WITH(FourCharCode, VTPGStringFromFourCharCodeOrUnsignedInt32);
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(long long,@"%lld");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(unsigned long long,@"%llu");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(float,@"%f");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(double,@"%f");
#if __has_feature(objc_arc)
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(__unsafe_unretained id,@"%@");
#else /* not __has_feature(objc_arc) */
    IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(id,@"%@");
#endif
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(short,@"%hi");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(unsigned short,@"%hu");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(int,@"%i");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(unsigned, @"%u");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(long,@"%ld");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(long double,@"%Lf"); //WARNING on older versions of OS X, @encode(long double) == @encode(double)

	//C-strings
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(char*, @"%s");
	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(const char*, @"%s");
	if(TypeCodeIsCharArray(typeCode))
		return [NSString stringWithFormat:@"%s", (char*)value];

	IF_TYPE_MATCHES_INTERPRET_WITH_FORMAT(void*,@"(void*)%p");

	//This is a hack to print out CLLocationCoordinate2D, without needing to #import <CoreLocation/CoreLocation.h>
	//A CLLocationCoordinate2D is a struct made up of 2 doubles.
	//We detect it by hard-coding the result of @encode(CLLocationCoordinate2D).
	//We get at the fields by treating it like an array of doubles, which it is identical to in memory.
	if(strcmp(typeCode, "{?=dd}")==0)//@encode(CLLocationCoordinate2D)
		return [NSString stringWithFormat:@"{latitude=%g,longitude=%g}",((double*)value)[0],((double*)value)[1]];

	//we don't know how to convert this typecode into an NSString
	return nil;
}

#else

#define DEBUG_LOG_EXPR(_X_)
#define DEBUG_LOG_FUNCTION()

#endif

// iOS specific macros
#if TARGET_OS_IPHONE

// Checking for device idiom
#define DEVICE_IS_PHONE()  (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
#define DEVICE_IS_PAD()    (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)

// Checking for iOS version
#define IOS_VERSION_EQ(v)   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedSame)
#define IOS_VERSION_GT(v)   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedDescending)
#define IOS_VERSION_GTE(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
#define IOS_VERSION_LT(v)   ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] == NSOrderedAscending)
#define IOS_VERSION_LTE(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedDescending)

#endif