// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP
#define POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP

#include "detail/ForwardDeclarations.hpp"
#include "RasterizerDescription.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeRasterizerState;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT RasterizerState {
public:
    RasterizerState() = delete;
    RasterizerState(RasterizerState const&) = delete;
    RasterizerState & operator=(RasterizerState const&) = delete;

    RasterizerState(std::shared_ptr<GraphicsDevice> const& graphicsDevice, RasterizerDescription const& description);

    ~RasterizerState();

    RasterizerDescription Description() const;

    static std::shared_ptr<RasterizerState>
    CreateCullClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<RasterizerState>
    CreateCullCounterClockwise(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    static std::shared_ptr<RasterizerState>
    CreateCullNone(std::shared_ptr<GraphicsDevice> const& graphicsDevice);

public:
    Detail::RenderSystem::NativeRasterizerState* NativeRasterizerState();

private:
    std::unique_ptr<Detail::RenderSystem::NativeRasterizerState> nativeRasterizerState;
    RasterizerDescription description;
};

}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERSTATE_BED8234F_ECEE_4505_972B_A1061D1EB4A6_HPP)
