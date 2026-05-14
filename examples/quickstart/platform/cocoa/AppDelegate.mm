#import "AppDelegate.h"
#include "game_setup.h"
#include "pomdog/application/cocoa/bootstrap_cocoa.h"
#include "pomdog/console/console.h"
#include "pomdog/pomdog.h"
#include <memory>
#if defined(POMDOG_DEBUG_BUILD)
#include <iostream>
#endif

using pomdog::Error;
using pomdog::Log;
using pomdog::LogEntry;
using pomdog::LogLevel;
using pomdog::ScopedConnection;

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow* window;

@end

@implementation AppDelegate {
    pomdog::cocoa::Bootstrap bootstrap;
#if defined(POMDOG_DEBUG_BUILD)
    ScopedConnection connection;
#endif
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
#if defined(POMDOG_DEBUG_BUILD)
    connection = Log::Connect([](const LogEntry& entry) {
        pomdog::console::write_line(entry.Message);
    });
    Log::SetLevel(LogLevel::Verbose);
#else
    Log::SetLevel(LogLevel::Critical);
#endif

    bootstrap.setWindow(self.window);

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        pomdog::console::write_error_line(err->toString());
    });

    bootstrap.onCompleted([=] {
        [self.window close];

        // Shutdown your application
        [NSApp terminate:nil];
    });

    if (auto err = bootstrap.run(quickstart::createGameSetup()); err != nullptr) {
#if defined(POMDOG_DEBUG_BUILD)
        pomdog::console::write_error_line(err->toString());
#endif
        // Shutdown your application
        [NSApp terminate:nil];
    }
}

- (void)applicationWillTerminate:(NSNotification*)aNotification
{
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

@end
