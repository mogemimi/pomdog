// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::GLTF {

class Document;

[[nodiscard]] POMDOG_EXPORT
std::tuple<std::shared_ptr<Document>, std::unique_ptr<Error>>
Open(const std::string& filePath) noexcept;

} // namespace Pomdog::GLTF
