// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTBUFFERBINDING_4C538B17_HPP
#define POMDOG_CONSTANTBUFFERBINDING_4C538B17_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Pomdog {

using ConstantBufferCollection = std::unordered_map<
    std::string, std::shared_ptr<ConstantBuffer>>;

class POMDOG_EXPORT ConstantBufferBinding {
public:
    ConstantBufferBinding() = delete;
    ConstantBufferBinding(ConstantBufferBinding const&) = delete;
    ConstantBufferBinding(ConstantBufferBinding &&) = default;
    ConstantBufferBinding & operator=(ConstantBufferBinding const&) = delete;
    ConstantBufferBinding & operator=(ConstantBufferBinding &&) = default;

    ConstantBufferBinding(
        GraphicsDevice & graphicsDevice,
        PipelineStateDescription const& pipelineStateDescription,
        PipelineState & pipelineState);

    ConstantBufferBinding(
        std::shared_ptr<GraphicsDevice> const& graphicsDevice,
        PipelineStateDescription const& pipelineStateDescription,
        PipelineState & pipelineState);

    ~ConstantBufferBinding();

    std::shared_ptr<ConstantBuffer>
    FindConstantBuffer(std::string const& name) const;

    void SetConstantBuffer(
        std::string const& name,
        std::shared_ptr<ConstantBuffer> const& constantBuffer);

    ConstantBufferCollection const& GetConstantBuffers() const;

public:
    Detail::NativeConstantLayout* NativeConstantLayout();

private:
    ConstantBufferCollection constantBuffers;
    std::unique_ptr<Detail::NativeConstantLayout> nativeConstantLayout;
};

} // namespace Pomdog

#endif // POMDOG_CONSTANTBUFFERBINDING_4C538B17_HPP
