// Based on: https://github.com/vasishb/lentil-user-app-additions/blob/aa2d6126a8f6f6215550a37b4f4afa6bd293223b/desktop-idle/src/mac/idle.mm (MIT)
#import <CoreFoundation/CoreFoundation.h>
#import <AppKit/AppKit.h>

extern "C" int getIdleTimeImpl()
{
	return CGEventSourceSecondsSinceLastEventType(kCGEventSourceStateHIDSystemState, kCGAnyInputEventType);
}
