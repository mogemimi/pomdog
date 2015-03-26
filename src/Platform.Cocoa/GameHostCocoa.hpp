// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEHOSTCOCOA_29C3B75B_HPP
#define POMDOG_GAMEHOSTCOCOA_29C3B75B_HPP

#include "../Application/SystemEventDispatcher.hpp"
#include "Pomdog/Application/GameHost.hpp"
#include <memory>

namespace Pomdog {

class PresentationParameters;

namespace Detail {
namespace Cocoa {

class GameWindowCocoa;

class GameHostCocoa final: public GameHost {
public:
    GameHostCocoa(std::shared_ptr<GameWindowCocoa> const& window,
        std::shared_ptr<SystemEventDispatcher> const& dispatcher,
        PresentationParameters const& presentationParameters);

    ~GameHostCocoa();

    ///@copydoc GameHost
    void Run(Game & game) override;

    ///@copydoc GameHost
    void Exit() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::GameWindow> Window() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::GameClock> Clock() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::GraphicsContext> GraphicsContext() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::GraphicsDevice> GraphicsDevice() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::AudioEngine> AudioEngine() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::AssetManager> AssetManager() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::Keyboard> Keyboard() override;

    ///@copydoc GameHost
    std::shared_ptr<Pomdog::Mouse> Mouse() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_GAMEHOSTCOCOA_29C3B75B_HPP
