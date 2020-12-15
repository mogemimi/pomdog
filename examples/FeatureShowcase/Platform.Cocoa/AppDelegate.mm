#import "AppDelegate.h"
#include "../Source/FeatureShowcaseGame.hpp"
#include <Pomdog/Platform/Cocoa/Bootstrap.hpp>
#include <Pomdog/Pomdog.hpp>
#include <memory>
#ifdef DEBUG
#include <iostream>
#endif

using Pomdog::Error;
using Pomdog::Game;
using Pomdog::GameHost;
using Pomdog::Log;
using Pomdog::LogEntry;
using Pomdog::LogLevel;
using Pomdog::ScopedConnection;

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow* window;

@end

@implementation AppDelegate {
    Pomdog::Cocoa::Bootstrap bootstrap;
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
        Log::Critical("Pomdog", err->ToString());
    });

    bootstrap.OnCompleted([=] {
        [self.window close];

        // Shutdown your application
        [NSApp terminate:nil];
    });

    auto createGame = [](const std::shared_ptr<GameHost>& gameHost) {
        return std::make_unique<FeatureShowcase::FeatureShowcaseGame>(gameHost);
    };

    if (auto err = bootstrap.Run(std::move(createGame)); err != nullptr) {
#ifdef DEBUG
        std::cerr << err->ToString() << std::endl;
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
