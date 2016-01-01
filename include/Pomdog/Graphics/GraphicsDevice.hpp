// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GraphicsDevice final {
public:
    GraphicsDevice() = delete;
    GraphicsDevice(GraphicsDevice const&) = delete;
    GraphicsDevice & operator=(GraphicsDevice const&) = delete;

    explicit GraphicsDevice(std::unique_ptr<Detail::NativeGraphicsDevice> && nativeDevice);

    ~GraphicsDevice();

    ShaderLanguage GetSupportedLanguage() const;

    Detail::NativeGraphicsDevice* GetNativeGraphicsDevice();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
