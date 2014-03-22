//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#import "AppDelegate.h"
#include <iostream>
#include <Pomdog/Pomdog.hpp>
#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include "../TestAppGame.hpp"

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
	bootstrapper.Run<TestApp::TestAppGame>([self window]);
}

@end
