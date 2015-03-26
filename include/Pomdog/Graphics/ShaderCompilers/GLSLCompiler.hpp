// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GLSLCOMPILER_9FED2927_CB1F_4D98_93D0_CC5E72F0C95B_HPP
#define POMDOG_GLSLCOMPILER_9FED2927_CB1F_4D98_93D0_CC5E72F0C95B_HPP

#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstddef>

namespace Pomdog {

class GraphicsDevice;
class Shader;

namespace ShaderCompilers {

struct POMDOG_EXPORT GLSLCompiler final {
    static std::unique_ptr<Shader> CreateVertexShader(GraphicsDevice & graphicsDevice,
        void const* shaderSource, std::size_t byteLength);

    static std::unique_ptr<Shader> CreatePixelShader(GraphicsDevice & graphicsDevice,
        void const* shaderSource, std::size_t byteLength);
};

}// namespace ShaderCompilers
}// namespace Pomdog

#endif // !defined(POMDOG_GLSLCOMPILER_9FED2927_CB1F_4D98_93D0_CC5E72F0C95B_HPP)
