// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEHOSTCOCOA_29C3B75B_HPP
#define POMDOG_GAMEHOSTCOCOA_29C3B75B_HPP

#include "Pomdog/Signals/EventQueue.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include <memory>

@class PomdogOpenGLView;

namespace Pomdog {

class Game;
struct PresentationParameters;

namespace Detail {
namespace Cocoa {

class GameWindowCocoa;

class GameHostCocoa final: public GameHost {
public:
    GameHostCocoa(
        PomdogOpenGLView* openGLView,
        std::shared_ptr<GameWindowCocoa> const& window,
        std::shared_ptr<EventQueue> const& eventQueue,
        PresentationParameters const& presentationParameters);

    ~GameHostCocoa();

    void Run(
        std::weak_ptr<Game> const& game,
        std::function<void()> const& onCompleted);

    void Exit() override;

    std::shared_ptr<Pomdog::GameWindow> Window() override;

    std::shared_ptr<Pomdog::GameClock> Clock() override;

    std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice() override;

    std::shared_ptr<Pomdog::GraphicsCommandQueue> GraphicsCommandQueue() override;

    std::shared_ptr<Pomdog::AudioEngine> AudioEngine() override;

    std::shared_ptr<Pomdog::AssetManager> AssetManager() override;

    std::shared_ptr<Pomdog::Keyboard> Keyboard() override;

    std::shared_ptr<Pomdog::Mouse> Mouse() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GAMEHOSTCOCOA_29C3B75B_HPP
