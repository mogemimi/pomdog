// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <string>

namespace Pomdog::MagicaVoxel {
class VoxModel;
} // namespace Pomdog::MagicaVoxel

namespace Pomdog::MagicaVoxel::VoxModelExporter {

[[nodiscard]] POMDOG_EXPORT
std::unique_ptr<Error>
Export(const VoxModel& model, const std::string& filePath) noexcept;

} // namespace Pomdog::MagicaVoxel::VoxModelExporter
