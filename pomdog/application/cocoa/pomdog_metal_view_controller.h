// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/signals/forward_declarations.h"
#import <Cocoa/Cocoa.h>
#import <MetalKit/MTKView.h>
#include <functional>
#include <memory>

namespace pomdog {
class Game;
class GameHost;
} // namespace pomdog

namespace pomdog::gpu {
class GraphicsDevice;
class CommandQueue;
} // namespace pomdog::gpu

@interface PomdogMetalViewController : NSViewController <MTKViewDelegate>

- (void)startGame:(std::function<std::shared_ptr<pomdog::Game>(const std::shared_ptr<pomdog::GameHost>&)>&&)createGame
        completed:(std::function<void()>&&)onCompleted;

- (void)setMouseCursor:(NSCursor*)cursor;

@end
