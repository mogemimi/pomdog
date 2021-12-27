// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::GLTF {

class Document;

[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<Document>, std::unique_ptr<Error>>
Open(const std::string& filePath) noexcept;

} // namespace pomdog::GLTF
