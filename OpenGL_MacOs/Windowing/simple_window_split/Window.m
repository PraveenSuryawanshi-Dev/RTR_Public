//headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "AppDelegate.h"


//Entry-point function

int main(int argc ,const char *argv[])
{
	NSAutoreleasePool *pPool= [[NSAutoreleasePool alloc]init];

	NSApp = [NSApplication sharedApplication];

	[NSApp setDelegate:[[AppDelegate alloc]init]];

	[NSApp run];

	[pPool release];

	return (0);
}


