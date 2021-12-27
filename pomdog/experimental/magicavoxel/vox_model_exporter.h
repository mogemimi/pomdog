// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::magicavoxel {
class VoxModel;
} // namespace pomdog::magicavoxel

namespace pomdog::magicavoxel::VoxModelExporter {

[[nodiscard]] POMDOG_EXPORT std::unique_ptr<Error>
Export(const VoxModel& model, const std::string& filePath) noexcept;

} // namespace pomdog::magicavoxel::VoxModelExporter
