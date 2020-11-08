// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/ForwardDeclarations.hpp"
#import <Cocoa/Cocoa.h>
#import <MetalKit/MTKView.h>
#include <functional>
#include <memory>

namespace Pomdog {

class Game;
class GameHost;
class GraphicsDevice;
class GraphicsCommandQueue;

} // namespace Pomdog

@interface PomdogMetalViewController : NSViewController <MTKViewDelegate>

- (void)startGame:(std::function<std::shared_ptr<Pomdog::Game>(const std::shared_ptr<Pomdog::GameHost>&)>&&)createGame
        completed:(std::function<void()>&&)onCompleted;

@end
