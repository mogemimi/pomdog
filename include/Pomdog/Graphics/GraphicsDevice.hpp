// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GraphicsDevice final {
public:
    GraphicsDevice() = delete;
    GraphicsDevice(const GraphicsDevice&) = delete;
    GraphicsDevice& operator=(const GraphicsDevice&) = delete;

    explicit GraphicsDevice(std::unique_ptr<Detail::NativeGraphicsDevice>&& nativeDevice);

    ~GraphicsDevice();

    /// Gets the currently supported shader language.
    ShaderLanguage GetSupportedLanguage() const noexcept;

    /// Gets the presentation parameters.
    PresentationParameters GetPresentationParameters() const noexcept;

    /// Gets the pointer of the native graphics device.
    Detail::NativeGraphicsDevice* GetNativeGraphicsDevice();

private:
    std::unique_ptr<Detail::NativeGraphicsDevice> nativeGraphicsDevice;
};

} // namespace Pomdog
