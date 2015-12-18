// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <string>

namespace Pomdog {
namespace MagicaVoxel {

class VoxModel;

struct VoxModelExporter {
    static void Export(
        MagicaVoxel::VoxModel const& model,
        std::string const& filePath);
};

} // namespace MagicaVoxel
} // namespace Pomdog
