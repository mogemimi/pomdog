#import "AppDelegate.h"
#include "game_main.h"
#include "pomdog/platform/cocoa/bootstrap_cocoa.h"
#include "pomdog/pomdog.h"
#include <memory>
#ifdef DEBUG
#include <iostream>
#endif

using pomdog::Error;
using pomdog::Game;
using pomdog::GameHost;
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

    bootstrap.SetWindow(self.window);
    bootstrap.SetOpenGLEnabled(false);

    bootstrap.OnError([](std::unique_ptr<Error>&& err) {
        Log::Critical("pomdog", err->toString());
    });

    bootstrap.OnCompleted([=] {
        [self.window close];

        // Shutdown your application
        [NSApp terminate:nil];
    });

    auto createGame = [](const std::shared_ptr<GameHost>& gameHost) {
        return std::make_unique<pong::GameMain>(gameHost);
    };

    if (auto err = bootstrap.Run(std::move(createGame)); err != nullptr) {
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
