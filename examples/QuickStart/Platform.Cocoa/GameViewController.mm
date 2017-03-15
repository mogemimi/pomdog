#import "GameViewController.h"
#include "QuickStartGame.hpp"
#include <memory>

@implementation GameViewController
{
    std::shared_ptr<Pomdog::Game> game;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)loadAssetsPomdog:(std::shared_ptr<Pomdog::GameHost>)gameHost
{
    game = std::make_shared<QuickStart::QuickStartGame>(gameHost);
    [self startGame:game];
}

@end
