// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <cstdint>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail {
class ShaderBytecode;
struct ShaderCompileOptions;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Direct3D {

[[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<ID3DBlob>, std::unique_ptr<Error>>
CompileHLSL(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions) noexcept;

} // namespace Pomdog::Detail::Direct3D
