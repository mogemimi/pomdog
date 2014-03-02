//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "AppDelegate.h"
#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include "QuickStartGame.hpp"

using Pomdog::Log;
using Pomdog::LogEntry;
using Pomdog::LoggingLevel;
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
		NSString* log = [NSString stringWithUTF8String:entry.message.c_str()];
		NSLog(@"\n%@", log);
	});
#endif

	Log::SetVerbosity(LoggingLevel::Internal);
	Log::LogMessage("applicationDidFinishLaunching");

	[[self window] makeKeyAndOrderFront:self];

	gameRunThread = [[NSThread alloc] initWithTarget:self selector:@selector(runGame) object:nil];
	[gameRunThread start];
	
	Log::LogMessage("game mainloop thread run");
}

- (void)runGame
{
	bootstrapper.Run<Pomdog::CocoaTestGame>([self window]);
}

@end
