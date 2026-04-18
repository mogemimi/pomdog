// Copyright mogemimi. Distributed under the MIT license.

#pragma once

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

- (void)startGame:(std::function<std::shared_ptr<pomdog::Game>()>&&)createGame
        completed:(std::function<void()>&&)onCompleted;

- (void)setMouseCursor:(NSCursor*)cursor;

@end
