// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include <functional>
#include <memory>

#import <AppKit/NSWindow.h>

@class PomdogOpenGLView;
@class PomdogMetalViewController;

namespace Pomdog {

class Game;
class GameHost;

namespace Detail {
namespace Cocoa {

class GameHostCocoa;
class GameHostMetal;

} // namespace Cocoa
} // namespace Detail

namespace Cocoa {

class POMDOG_EXPORT Bootstrap final {
public:
    void SetWindow(NSWindow* window);

    void SetOpenGLEnabled(bool enabled);

    void SetOpenGLSurfaceFormat(SurfaceFormat surfaceFormat);

    void SetOpenGLDepthFormat(DepthFormat depthFormat);

    void OnError(std::function<void(const std::exception&)>&& onError);

    void OnCompleted(std::function<void()>&& onCompleted);

    void Run(std::function<std::shared_ptr<Game>(const std::shared_ptr<GameHost>&)>&& createGame);

private:
    std::function<void()> onCompleted;
    std::function<void(const std::exception&)> onError;
    std::shared_ptr<Pomdog::Detail::Cocoa::GameHostCocoa> gameHostCocoa;
    std::shared_ptr<Pomdog::Detail::Cocoa::GameHostMetal> gameHostMetal;
    std::shared_ptr<Game> game;
    __weak NSWindow* nativeWindow = nil;
    PomdogMetalViewController* viewController = nil;
    SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    DepthFormat depthFormat = DepthFormat::Depth24Stencil8;
    bool openGLEnabled = false;
};

} // namespace Cocoa
} // namespace Pomdog
