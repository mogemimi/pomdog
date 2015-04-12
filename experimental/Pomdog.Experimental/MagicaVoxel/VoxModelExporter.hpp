// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VOXMODELEXPORTER_F3E3A4F5_HPP
#define POMDOG_VOXMODELEXPORTER_F3E3A4F5_HPP

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

#endif // POMDOG_VOXMODELEXPORTER_F3E3A4F5_HPP
