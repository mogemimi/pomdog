// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTLAYOUTDIRECT3D11_7DB9A007_HPP
#define POMDOG_CONSTANTLAYOUTDIRECT3D11_7DB9A007_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeConstantLayout.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

struct ConstantBufferBindingDirect3D11 {
    std::shared_ptr<HardwareBufferDirect3D11> ConstantBuffer;
    std::string Name;
    std::uint16_t StartSlot;
};

class ConstantLayoutDirect3D11 final: public NativeConstantLayout {
public:
    explicit ConstantLayoutDirect3D11(
        std::vector<ConstantBufferBindingDirect3D11> && bindings);

    void SetConstantBuffer(std::string const& constantName,
        std::shared_ptr<NativeBuffer> const& constantBuffer) override;

    void SetConstantBuffer(std::string const& constantName) override;

    void Apply(ID3D11DeviceContext * deviceContext);

private:
    std::vector<ConstantBufferBindingDirect3D11> bindings;
};

} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_CONSTANTLAYOUTDIRECT3D11_7DB9A007_HPP
