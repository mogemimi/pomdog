// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_BOOTSTRAPPERCOCOA_13B44EFA_HPP
#define POMDOG_BOOTSTRAPPERCOCOA_13B44EFA_HPP

#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <functional>
#include <memory>

#import <AppKit/NSWindow.h>

@class PomdogOpenGLView;

namespace Pomdog {

class Game;
class GameHost;

namespace Detail {
namespace Cocoa {

class GameHostCocoa;

} // namespace Cocoa
} // namespace Detail

namespace Cocoa {

class POMDOG_EXPORT Bootstrap final {
public:
    void SetView(PomdogOpenGLView* openGLView);

    void SetSurfaceFormat(SurfaceFormat surfaceFormat);

    void SetDepthFormat(DepthFormat depthFormat);

    void OnError(std::function<void(std::exception const&)> onError);

    void OnCompleted(std::function<void()> onCompleted);

    void Run(std::function<std::unique_ptr<Game>(
        std::shared_ptr<GameHost> const&)> const& createApp);

private:
    std::function<void()> onCompleted;
    std::function<void(std::exception const&)> onError;
    std::shared_ptr<Pomdog::Detail::Cocoa::GameHostCocoa> gameHost;
    std::shared_ptr<Game> game;
    __weak PomdogOpenGLView* openGLView = nil;
    SurfaceFormat surfaceFormat = SurfaceFormat::R8G8B8A8_UNorm;
    DepthFormat depthFormat = DepthFormat::Depth24Stencil8;
};

} // namespace Cocoa
} // namespace Pomdog

#endif // POMDOG_BOOTSTRAPPERCOCOA_13B44EFA_HPP
