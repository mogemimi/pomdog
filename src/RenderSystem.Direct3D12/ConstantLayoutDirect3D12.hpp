// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTLAYOUTDIRECT3D12_28F04DD9_HPP
#define POMDOG_CONSTANTLAYOUTDIRECT3D12_28F04DD9_HPP

#include "../RenderSystem/NativeConstantLayout.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <wrl/client.h>
#include <d3d12.h>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {

class BufferDirect3D12;

struct ConstantBufferView {
    std::shared_ptr<BufferDirect3D12> ConstantBuffer;
    std::string Name;
    UINT SlotIndex; /// @todo not used
};

class ConstantLayoutDirect3D12 final : public NativeConstantLayout {
public:
    explicit ConstantLayoutDirect3D12(std::vector<ConstantBufferView> && views);

    void SetConstantBuffer(std::string const& constantName,
        std::shared_ptr<NativeBuffer> const& constantBuffer) override;

    void SetConstantBuffer(std::string const& constantName) override;

    std::vector<ConstantBufferView> const& GetConstantBufferViews() const noexcept;

private:
    std::vector<ConstantBufferView> views;
};

} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_CONSTANTLAYOUTDIRECT3D12_28F04DD9_HPP
