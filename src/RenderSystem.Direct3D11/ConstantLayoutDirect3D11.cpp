// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ConstantLayoutDirect3D11.hpp"
#include "HardwareBufferDirect3D11.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
//-----------------------------------------------------------------------
ConstantLayoutDirect3D11::ConstantLayoutDirect3D11(
    std::vector<ConstantBufferBindingDirect3D11> && bindingsIn)
    : bindings(std::move(bindingsIn))
{}
//-----------------------------------------------------------------------
void ConstantLayoutDirect3D11::SetConstantBuffer(
    std::string const& constantName,
    std::shared_ptr<NativeBuffer> const& constantBuffer)
{
    auto iter = std::find_if(std::begin(bindings), std::end(bindings),
        [&](ConstantBufferBindingDirect3D11 const& binding) {
            return binding.Name == constantName;
        });

    POMDOG_ASSERT(std::end(bindings) != iter);

    auto nativeConstantBuffer = std::dynamic_pointer_cast<HardwareBufferDirect3D11>(constantBuffer);
    POMDOG_ASSERT(nativeConstantBuffer);

    if (std::end(bindings) != iter) {
        iter->ConstantBuffer = nativeConstantBuffer;
    }
}
//-----------------------------------------------------------------------
void ConstantLayoutDirect3D11::SetConstantBuffer(std::string const& constantName)
{
    auto iter = std::find_if(std::begin(bindings), std::end(bindings),
        [&](ConstantBufferBindingDirect3D11 const& binding) {
            return binding.Name == constantName;
        });

    POMDOG_ASSERT(std::end(bindings) != iter);

    if (std::end(bindings) != iter) {
        iter->ConstantBuffer.reset();
    }
}
//-----------------------------------------------------------------------
void ConstantLayoutDirect3D11::Apply(ID3D11DeviceContext * deviceContext)
{
    for (auto & binding: bindings)
    {
        auto constantBuffer = binding.ConstantBuffer->GetBuffer();
        POMDOG_ASSERT(constantBuffer);
        deviceContext->VSSetConstantBuffers(binding.StartSlot, 1, &constantBuffer);
        deviceContext->PSSetConstantBuffers(binding.StartSlot, 1, &constantBuffer);
    }
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
