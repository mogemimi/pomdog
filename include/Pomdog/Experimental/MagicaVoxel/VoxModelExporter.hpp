// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <string>

namespace Pomdog {
namespace MagicaVoxel {

class VoxModel;

struct POMDOG_EXPORT VoxModelExporter final {
    static void Export(
        const MagicaVoxel::VoxModel& model,
        const std::string& filePath);
};

} // namespace MagicaVoxel
} // namespace Pomdog
