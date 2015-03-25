// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ConstantLayoutGL4.hpp"
#include "ConstantBufferGL4.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

ConstantLayoutGL4::ConstantLayoutGL4(std::vector<ConstantBufferBindingGL4> && bindingsIn)
    : bindings(std::move(bindingsIn))
{}
//-----------------------------------------------------------------------
void ConstantLayoutGL4::SetConstantBuffer(std::string const& constantName, std::shared_ptr<NativeConstantBuffer> const& constantBuffer)
{
    auto iter = std::find_if(std::begin(bindings), std::end(bindings),
        [&constantName](ConstantBufferBindingGL4 const& binding) { return binding.Name == constantName; });

    POMDOG_ASSERT(std::end(bindings) != iter);

    auto nativeConstantBuffer = std::dynamic_pointer_cast<ConstantBufferGL4>(constantBuffer);
    POMDOG_ASSERT(nativeConstantBuffer);

    if (std::end(bindings) != iter) {
        iter->ConstantBuffer = nativeConstantBuffer;
    }
}
//-----------------------------------------------------------------------
void ConstantLayoutGL4::SetConstantBuffer(std::string const& constantName)
{
    auto iter = std::find_if(std::begin(bindings), std::end(bindings),
        [&constantName](ConstantBufferBindingGL4 const& binding) { return binding.Name == constantName; });

    POMDOG_ASSERT(std::end(bindings) != iter);

    if (std::end(bindings) != iter) {
        iter->ConstantBuffer.reset();
    }
}
//-----------------------------------------------------------------------
void ConstantLayoutGL4::Apply()
{
    for (auto & binding: bindings)
    {
        if (binding.ConstantBuffer) {
            binding.ConstantBuffer->Apply(binding.SlotIndex);
        }
    }
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
