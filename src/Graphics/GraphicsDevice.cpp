// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

GraphicsDevice::GraphicsDevice(std::unique_ptr<Detail::NativeGraphicsDevice> && device)
    : nativeGraphicsDevice(std::move(device))
{
    POMDOG_ASSERT(nativeGraphicsDevice);
}

GraphicsDevice::~GraphicsDevice() = default;

ShaderLanguage GraphicsDevice::GetSupportedLanguage() const noexcept
{
    POMDOG_ASSERT(nativeGraphicsDevice);
    return nativeGraphicsDevice->GetSupportedLanguage();
}

PresentationParameters GraphicsDevice::GetPresentationParameters() const noexcept
{
    POMDOG_ASSERT(nativeGraphicsDevice);
    return nativeGraphicsDevice->GetPresentationParameters();
}

Detail::NativeGraphicsDevice* GraphicsDevice::GetNativeGraphicsDevice()
{
    POMDOG_ASSERT(nativeGraphicsDevice);
    return nativeGraphicsDevice.get();
}

} // namespace Pomdog
