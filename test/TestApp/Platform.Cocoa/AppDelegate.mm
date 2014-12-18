//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#import "AppDelegate.h"
#include "../TestAppGame.hpp"
#include <Pomdog/Application/detail/Platform.Cocoa/BootstrapperCocoa.hpp>
#include <Pomdog/Pomdog.hpp>
#include <iostream>

using Pomdog::GameHost;
using Pomdog::Log;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate
{
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

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

- (void)runGame
{
	using Bootstrapper = Pomdog::Details::Cocoa::BootstrapperCocoa;

	Bootstrapper bootstrapper([self window]);
	bootstrapper.Run([](std::shared_ptr<GameHost> const& gameHost)
	{
		try {
			TestApp::TestAppGame game{gameHost};
			gameHost->Run(game);
		}
		catch (std::exception const& e) {
			Log::Critical("Pomdog", e.what());
		}
	});
	
	///@note Shutdown your application
	[NSApp terminate:nil];
}

@end
