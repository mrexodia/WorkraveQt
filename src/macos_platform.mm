#import <CoreFoundation/CoreFoundation.h>
#import <AppKit/AppKit.h>
// Based on: https://github.com/vasishb/lentil-user-app-additions/blob/aa2d6126a8f6f6215550a37b4f4afa6bd293223b/desktop-idle/src/mac/idle.mm (MIT)

extern "C" int getIdleTimeImpl()
{
	return CGEventSourceSecondsSinceLastEventType(kCGEventSourceStateHIDSystemState, kCGAnyInputEventType);
}

// Based on: https://stackoverflow.com/a/26489672
static OSStatus MDSendAppleEventToSystemProcess(AEEventID eventToSendID)
{
	AEAddressDesc                    targetDesc;
	static const ProcessSerialNumber kPSNOfSystemProcess = {0, kSystemProcess};
	AppleEvent                       eventReply          = {typeNull, NULL};
	AppleEvent                       eventToSend         = {typeNull, NULL};

	OSStatus status = AECreateDesc(typeProcessSerialNumber,
			&kPSNOfSystemProcess, sizeof(kPSNOfSystemProcess), &targetDesc);

	if ( status != noErr ) return status;

	status = AECreateAppleEvent(kCoreEventClass, eventToSendID,
			&targetDesc, kAutoGenerateReturnID, kAnyTransactionID, &eventToSend);

	AEDisposeDesc(&targetDesc);

	if ( status != noErr ) return status;

	status = AESendMessage(&eventToSend, &eventReply,
			kAENormalPriority, kAEDefaultTimeout);

	AEDisposeDesc(&eventToSend);
	if ( status != noErr ) return status;
	AEDisposeDesc(&eventReply);
	return status;
}

extern "C" void lockScreenImpl()
{
	MDSendAppleEventToSystemProcess(kAESleep);
}
