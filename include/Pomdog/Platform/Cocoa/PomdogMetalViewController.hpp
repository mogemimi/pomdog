// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include <memory>
#include <functional>

#import <Cocoa/Cocoa.h>
#import <MetalKit/MTKView.h>

namespace Pomdog {

class Game;
class GameHost;
class GraphicsDevice;
class GraphicsCommandQueue;

} // namespace Pomdog

POMDOG_EXPORT
@interface PomdogMetalViewController : NSViewController<MTKViewDelegate>

- (void)loadAssetsPomdog:(std::shared_ptr<Pomdog::GameHost>)gameHost;
- (void)startGame:(std::shared_ptr<Pomdog::Game>)game;

@end
