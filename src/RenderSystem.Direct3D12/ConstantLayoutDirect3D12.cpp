// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "ConstantLayoutDirect3D12.hpp"
#include "BufferDirect3D12.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
//-----------------------------------------------------------------------
ConstantLayoutDirect3D12::ConstantLayoutDirect3D12(
    std::vector<ConstantBufferView> && viewsIn)
    : views(std::move(viewsIn))
{}
//-----------------------------------------------------------------------
void ConstantLayoutDirect3D12::SetConstantBuffer(std::string const& constantName,
    std::shared_ptr<NativeBuffer> const& constantBufferIn)
{
    POMDOG_ASSERT(!views.empty());
    if (views.empty()) {
        return;
    }

    auto view = std::find_if(std::begin(views), std::end(views),
        [&](ConstantBufferView const& view) {
            return view.Name == constantName;
        });

    POMDOG_ASSERT(std::end(views) != view);
    if (std::end(views) == view) {
        return;
    }

    view->ConstantBuffer = std::dynamic_pointer_cast<BufferDirect3D12>(constantBufferIn);
    POMDOG_ASSERT(view->ConstantBuffer);
}
//-----------------------------------------------------------------------
void ConstantLayoutDirect3D12::SetConstantBuffer(std::string const& constantName)
{
    POMDOG_ASSERT(!views.empty());
    if (views.empty()) {
        return;
    }

    auto view = std::find_if(std::begin(views), std::end(views),
        [&](ConstantBufferView const& view) {
        return view.Name == constantName;
    });

    POMDOG_ASSERT(std::end(views) != view);
    if (std::end(views) == view) {
        return;
    }

    view->ConstantBuffer.reset();
}
//-----------------------------------------------------------------------
std::vector<ConstantBufferView> const& ConstantLayoutDirect3D12::GetConstantBufferViews() const noexcept
{
    return views;
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
