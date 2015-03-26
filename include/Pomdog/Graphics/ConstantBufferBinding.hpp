// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONSTANTBUFFERBINDING_4C538B17_4EF0_47AC_B3BA_2369FD5A0540_HPP
#define POMDOG_CONSTANTBUFFERBINDING_4C538B17_4EF0_47AC_B3BA_2369FD5A0540_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <unordered_map>
#include <string>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {

class NativeConstantLayout;

}// namespace RenderSystem
}// namespace Detail

using EffectParameterCollection = std::unordered_map<std::string, std::shared_ptr<EffectParameter>>;

class POMDOG_EXPORT ConstantBufferBinding {
public:
    ConstantBufferBinding() = delete;
    ConstantBufferBinding(ConstantBufferBinding const&) = delete;
    ConstantBufferBinding(ConstantBufferBinding &&) = default;

    ConstantBufferBinding(GraphicsDevice & graphicsDevice, EffectPass & effectPass);

    ConstantBufferBinding(std::shared_ptr<GraphicsDevice> const& graphicsDevice, EffectPass & effectPass);

    ~ConstantBufferBinding();

    ConstantBufferBinding & operator=(ConstantBufferBinding const&) = delete;
    ConstantBufferBinding & operator=(ConstantBufferBinding &&) = default;

    std::shared_ptr<EffectParameter> const& Find(std::string const& parameterName) const;

    EffectParameterCollection const& Find() const;

public:
    Detail::RenderSystem::NativeConstantLayout* NativeConstantLayout();

private:
    EffectParameterCollection effectParameters;
    std::unique_ptr<Detail::RenderSystem::NativeConstantLayout> nativeConstantLayout;
};

}// namespace Pomdog

#endif // !defined(POMDOG_CONSTANTBUFFERBINDING_4C538B17_4EF0_47AC_B3BA_2369FD5A0540_HPP)
