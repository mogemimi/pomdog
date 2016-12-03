#import "GameViewController.h"
#import <Metal/Metal.h>
#import <simd/simd.h>
#import <MetalKit/MetalKit.h>
#import "SharedStructures.h"
#include <Pomdog/Math/Vector3.hpp>
#include <Pomdog/Math/Vector4.hpp>
#include <Pomdog/Math/Matrix3x3.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include <Pomdog/Math/Quaternion.hpp>
#include <Pomdog/Math/Radian.hpp>
#include <Pomdog/Graphics/ConstantBuffer.hpp>
#include <Pomdog/Graphics/GraphicsDevice.hpp>
#include <Pomdog/Graphics/PipelineState.hpp>
#include <Pomdog/Graphics/VertexBuffer.hpp>
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
