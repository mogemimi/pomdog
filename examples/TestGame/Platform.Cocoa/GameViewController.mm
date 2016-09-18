#import "GameViewController.h"
#include "../Source/TestGame.hpp"
#include <Pomdog/Platform/Cocoa/Bootstrap.hpp>
#include <Pomdog/Pomdog.hpp>
#ifdef DEBUG
#include <iostream>
#endif

@implementation GameViewController
{
#ifdef DEBUG
    Pomdog::ScopedConnection connection;
#endif
    Pomdog::Cocoa::Bootstrap bootstrap;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    using Pomdog::Log;
    using Pomdog::LogLevel;

#ifdef DEBUG
    connection = Log::Connect([](Pomdog::LogEntry const& entry) {
        std::cout << entry.Message << std::endl;
    });
    Log::SetLevel(LogLevel::Verbose);
#else
    Log::SetLevel(LogLevel::Critical);
#endif

    bootstrap.SetView([self gameView]);
    bootstrap.OnCompleted([=] {
        [[[self gameView] window] close];

        // Shutdown your application
        [NSApp terminate:nil];
    });

    bootstrap.Run([](std::shared_ptr<Pomdog::GameHost> const& gameHost) {
        return std::make_unique<Pomdog::TestGame>(gameHost);
    });
}

@end
