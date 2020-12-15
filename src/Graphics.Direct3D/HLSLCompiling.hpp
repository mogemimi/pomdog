// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D.hpp"
#include <wrl/client.h>
#include <cstdint>
#include <memory>
#include <tuple>

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D {

[[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, std::shared_ptr<Error>>
CompileHLSL(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept;

} // namespace Pomdog::Detail::Direct3D
