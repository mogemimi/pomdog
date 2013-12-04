//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "AppDelegate.h"
#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include "CocoaTestGame.hpp"

using Pomdog::Log;
using Pomdog::LogEntry;
using LogConnection = Pomdog::ScopedConnection<Pomdog::EventConnection>;
using Bootstrapper = Pomdog::Details::Cocoa::BootstrapperCocoa;

@implementation AppDelegate
{
	Bootstrapper bootstrapper;
	LogConnection connection;
	
	NSThread* gameRunThread;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
#ifdef DEBUG
	connection.Reset(Log::Connect([](Pomdog::LogEntry entry) {
		NSString* log = [NSString stringWithUTF8String:entry.message.c_str()];
		NSLog(@"%@", log);
	}));
#endif

	Log::LogMessage("applicationDidFinishLaunching");

	bootstrapper.BuildOpenGLView([self window]);
	[[self window] makeKeyAndOrderFront:self];

	gameRunThread = [[NSThread alloc] initWithTarget:self selector:@selector(runGame) object:nil];
	[gameRunThread start];

	Log::LogMessage("game mainloop thread run");
}

//- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
//{
//	return YES;
//}

- (void)runGame
{
	bootstrapper.Run<Pomdog::CocoaTestGame>();
}

@end
