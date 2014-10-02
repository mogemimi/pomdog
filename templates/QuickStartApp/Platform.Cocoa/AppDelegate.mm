#import "AppDelegate.h"

#include "QuickStartGame.hpp"
#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include <Pomdog/Pomdog.hpp>
#include <iostream>

using Pomdog::Log;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;
using Bootstrapper = Pomdog::Details::Cocoa::BootstrapperCocoa;

@implementation AppDelegate
{
	Bootstrapper bootstrapper;
	ScopedConnection connection;

	NSThread* gameRunThread;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
#ifdef DEBUG
	connection = Log::Connect([](Pomdog::LogEntry const& entry) {
		//NSString* log = [NSString stringWithUTF8String:entry.Message.c_str()];
		//NSLog(@"\n%@", log);
		std::cout << entry.Message << "\n";
	});
#endif

	Log::SetLevel(LogLevel::Internal);
	Log::Verbose("applicationDidFinishLaunching");

	[[self window] makeKeyAndOrderFront:self];

	gameRunThread = [[NSThread alloc] initWithTarget:self selector:@selector(runGame) object:nil];
	[gameRunThread start];

	Log::Verbose("game mainloop thread run");
}

- (void)runGame
{
	bootstrapper.Run<QuickStart::QuickStartGame>([self window]);
}

@end
