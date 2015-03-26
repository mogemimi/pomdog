// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_HLSLCOMPILER_223CB751_HPP
#define POMDOG_HLSLCOMPILER_223CB751_HPP

#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstddef>
#include <string>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT HLSLCompiler final {
    static std::unique_ptr<Shader> CreateVertexShader(GraphicsDevice & graphicsDevice,
        void const* shaderSource, std::size_t byteLength);

    static std::unique_ptr<Shader> CreatePixelShader(GraphicsDevice & graphicsDevice,
        void const* shaderSource, std::size_t byteLength);

    static std::unique_ptr<Shader> CreateVertexShaderFromSource(GraphicsDevice & graphicsDevice,
        void const* shaderSource, std::size_t byteLength,
        std::string const& entryPoint);

    static std::unique_ptr<Shader> CreatePixelShaderFromSource(GraphicsDevice & graphicsDevice,
        void const* shaderSource, std::size_t byteLength,
        std::string const& entryPoint);
};

}// namespace ShaderCompilers
}// namespace Pomdog

#endif // POMDOG_HLSLCOMPILER_223CB751_HPP
