// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include <functional>
#include <memory>
#import <Cocoa/Cocoa.h>
#import <MetalKit/MTKView.h>

namespace Pomdog {

class Game;
class GameHost;
class GraphicsDevice;
class GraphicsCommandQueue;

} // namespace Pomdog

@interface PomdogMetalViewController : NSViewController<MTKViewDelegate>

- (void)startGame:(std::function<std::shared_ptr<Pomdog::Game>(const std::shared_ptr<Pomdog::GameHost>&)>&&)createGame
    completed:(std::function<void()>&&) onCompleted;

@end
