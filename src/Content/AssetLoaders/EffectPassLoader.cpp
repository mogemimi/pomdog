// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/AssetLoaders/EffectPassLoader.hpp"
#include "../../Utility/PathHelper.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Graphics/ShaderCompilers/GLSLCompiler.hpp"
#include "Pomdog/Graphics/ShaderCompilers/HLSLCompiler.hpp"
#include "Pomdog/Graphics/EffectPass.hpp"
#include "Pomdog/Graphics/EffectPassDescription.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/Shader.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace Pomdog {

using ShaderCompilers::GLSLCompiler;
using ShaderCompilers::HLSLCompiler;

namespace AssetLoaders {
namespace {

static std::vector<std::uint8_t> ReadBinaryFile(std::ifstream && streamIn)
{
    std::ifstream stream = std::move(streamIn);

    if (!stream) {
        return {};
    }

    stream.seekg(0, stream.end);
    auto const length = static_cast<std::size_t>(stream.tellg());
    stream.seekg(0, stream.beg);

    std::vector<std::uint8_t> result(length + 1, 0);
    stream.read(reinterpret_cast<char*>(result.data()), result.size());
    return std::move(result);
}

}// unnamed namespace
//-----------------------------------------------------------------------
class EffectPassLoader::Impl final {
public:
    EffectPassDescription description;
    Detail::AssetLoaderContext loaderContext;
    std::shared_ptr<GraphicsDevice> graphicsDevice;
    bool hasBlendState = false;
    bool hasRasterizerState = false;
    bool hasDepthStencilState = false;

public:
    std::shared_ptr<EffectPass> Load();
};
//-----------------------------------------------------------------------
std::shared_ptr<EffectPass> EffectPassLoader::Impl::Load()
{
    POMDOG_ASSERT(!description.InputLayout.InputElements.empty());

    if (!hasBlendState) {
        description.BlendState = BlendDescription::CreateDefault();
        hasBlendState = true;
    }

    if (!hasRasterizerState) {
        description.RasterizerState = RasterizerDescription::CreateDefault();
        hasRasterizerState = true;
    }

    if (!hasDepthStencilState) {
        description.DepthStencilState = DepthStencilDescription::CreateDefault();
        hasDepthStencilState = true;
    }

    auto effectPass = std::make_shared<EffectPass>(graphicsDevice, description);
    return std::move(effectPass);
}
//-----------------------------------------------------------------------
EffectPassLoader::EffectPassLoader(Detail::AssetLoaderContext const& loaderContextIn)
    : impl(std::make_unique<Impl>())
{
    POMDOG_ASSERT(impl);
    impl->loaderContext = loaderContextIn;
    impl->graphicsDevice = loaderContextIn.GraphicsDevice.lock();
    POMDOG_ASSERT(impl->graphicsDevice);
}
//-----------------------------------------------------------------------
EffectPassLoader::EffectPassLoader(EffectPassLoader &&) = default;
EffectPassLoader & EffectPassLoader::operator=(EffectPassLoader &&) = default;
EffectPassLoader::~EffectPassLoader() = default;
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::VertexShaderGLSL(std::string const& filePath)
{
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        auto shaderSource = ReadBinaryFile(impl->loaderContext.OpenStream(filePath));
        if (shaderSource.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
        }

        impl->description.VertexShader = GLSLCompiler::CreateVertexShader(
            *impl->graphicsDevice, shaderSource.data(), shaderSource.size());
    }
    return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::PixelShaderGLSL(std::string const& filePath)
{
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice->GetSupportedLanguage() == ShaderLanguage::GLSL)
    {
        auto shaderSource = ReadBinaryFile(impl->loaderContext.OpenStream(filePath));
        if (shaderSource.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
        }

        impl->description.PixelShader = GLSLCompiler::CreatePixelShader(
            *impl->graphicsDevice, shaderSource.data(), shaderSource.size());
    }
    return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::VertexShaderHLSL(std::string const& filePath, std::string const& entryPoint)
{
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        auto shaderSource = ReadBinaryFile(impl->loaderContext.OpenStream(filePath));
        if (shaderSource.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
        }

        impl->description.VertexShader = HLSLCompiler::CreateVertexShaderFromSource(
            *impl->graphicsDevice, shaderSource.data(), shaderSource.size(), entryPoint);
    }
    return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::PixelShaderHLSL(std::string const& filePath, std::string const& entryPoint)
{
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(!entryPoint.empty());
    POMDOG_ASSERT(impl);

    if (impl->graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        auto shaderSource = ReadBinaryFile(impl->loaderContext.OpenStream(filePath));
        if (shaderSource.empty()) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file.");
        }

        impl->description.PixelShader = HLSLCompiler::CreatePixelShaderFromSource(
            *impl->graphicsDevice, shaderSource.data(), shaderSource.size(), entryPoint);
    }
    return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::InputLayout(InputLayoutDescription const& inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->description.InputLayout = inputLayout;
    return *this;
}
//-----------------------------------------------------------------------
EffectPassLoader & EffectPassLoader::InputLayout(InputLayoutDescription && inputLayout)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(!inputLayout.InputElements.empty());
    impl->description.InputLayout = std::move(inputLayout);
    return *this;
}
//-----------------------------------------------------------------------
std::shared_ptr<EffectPass> EffectPassLoader::Load()
{
    POMDOG_ASSERT(impl);
    return impl->Load();
}
//-----------------------------------------------------------------------
}// namespace AssetLoaders
}// namespace Pomdog
