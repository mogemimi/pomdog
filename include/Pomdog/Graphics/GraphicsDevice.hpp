// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT GraphicsDevice final {
public:
    GraphicsDevice() = delete;
    GraphicsDevice(const GraphicsDevice&) = delete;
    GraphicsDevice& operator=(const GraphicsDevice&) = delete;

    explicit GraphicsDevice(std::unique_ptr<Detail::NativeGraphicsDevice>&& nativeDevice);

    ~GraphicsDevice();

    ShaderLanguage GetSupportedLanguage() const noexcept;

    PresentationParameters GetPresentationParameters() const noexcept;

    Detail::NativeGraphicsDevice* GetNativeGraphicsDevice();

private:
    std::unique_ptr<Detail::NativeGraphicsDevice> nativeGraphicsDevice;
};

} // namespace Pomdog
