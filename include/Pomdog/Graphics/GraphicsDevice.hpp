// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSDEVICE_7A54711C_HPP
#define POMDOG_GRAPHICSDEVICE_7A54711C_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {

class BuiltinShaderPool;

} // namespace Detail

class POMDOG_EXPORT GraphicsDevice {
public:
    GraphicsDevice() = delete;
    GraphicsDevice(GraphicsDevice const&) = delete;
    GraphicsDevice & operator=(GraphicsDevice const&) = delete;

    explicit GraphicsDevice(std::unique_ptr<Detail::RenderSystem::NativeGraphicsDevice> && nativeDevice);

    ~GraphicsDevice();

    ShaderLanguage GetSupportedLanguage() const;

    Detail::BuiltinShaderPool & ShaderPool();

public:
    ///@brief internal method
    Detail::RenderSystem::NativeGraphicsDevice* NativeGraphicsDevice();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog

#endif // POMDOG_GRAPHICSDEVICE_7A54711C_HPP
