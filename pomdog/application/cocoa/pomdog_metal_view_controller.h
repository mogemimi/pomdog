// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#import <MetalKit/MTKView.h>
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
