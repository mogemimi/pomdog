// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/direct3d/prerequisites_direct3d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <cstdint>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace pomdog::detail

namespace pomdog::detail::direct3d {

[[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, std::unique_ptr<Error>>
CompileHLSL(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept;

} // namespace pomdog::detail::direct3d
