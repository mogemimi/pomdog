#import "AppDelegate.h"
#include "game_setup.h"
#include "pomdog/platform/cocoa/bootstrap_cocoa.h"
#include "pomdog/pomdog.h"
#include <memory>
#ifdef DEBUG
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
#ifdef DEBUG
    ScopedConnection connection;
#endif
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
#ifdef DEBUG
    connection = Log::Connect([](const LogEntry& entry) {
        std::cout << entry.Message << std::endl;
    });
    Log::SetLevel(LogLevel::Verbose);
#else
    Log::SetLevel(LogLevel::Critical);
#endif

    bootstrap.setWindow(self.window);

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->toString());
    });

    bootstrap.onCompleted([=] {
        [self.window close];

        // Shutdown your application
        [NSApp terminate:nil];
    });

    if (auto err = bootstrap.run(feature_showcase::createGameSetup()); err != nullptr) {
#ifdef DEBUG
        std::cerr << err->toString() << std::endl;
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
