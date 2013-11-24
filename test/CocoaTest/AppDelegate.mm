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

using Pomdog::Log;
using Pomdog::LogEntry;
using LogConnection = Pomdog::ScopedConnection<Pomdog::EventConnection>;

@implementation AppDelegate
{
	LogConnection connection;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	
	connection.Reset(Log::Connect([](Pomdog::LogEntry entry) {
		NSString* log = [NSString stringWithUTF8String:entry.message.c_str()];
		NSLog(@"[Pomdog] %@", log);
	}));
	
	Log::LogMessage("applicationDidFinishLaunching");
	
	//std::shared_ptr<GameSystem> gameSystem;
	//gameSystem->
}

@end
