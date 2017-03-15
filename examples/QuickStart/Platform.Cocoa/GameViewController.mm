#import "GameViewController.h"
#include "QuickStartGame.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>
#ifdef DEBUG
#include <iostream>
#endif

@implementation GameViewController
{
    std::shared_ptr<Pomdog::Game> game;
#ifdef DEBUG
    Pomdog::ScopedConnection connection;
#endif
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
}

- (void)loadAssetsPomdog:(std::shared_ptr<Pomdog::GameHost>)gameHost
{
    game = std::make_shared<QuickStart::QuickStartGame>(gameHost);
    [self startGame:game];
}

@end
