// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VOXMODELEXPORTER_F3E3A4F5_F454_4E43_B1BB_BF0E9F237B54_HPP
#define POMDOG_VOXMODELEXPORTER_F3E3A4F5_F454_4E43_B1BB_BF0E9F237B54_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <string>

namespace Pomdog {
namespace MagicaVoxel {

class VoxModel;

class VoxModelExporter {
public:
	static void Export(MagicaVoxel::VoxModel const& model, std::string const& filePath);
};

}// namespace MagicaVoxel
}// namespace Pomdog

#endif // !defined(POMDOG_VOXMODELEXPORTER_F3E3A4F5_F454_4E43_B1BB_BF0E9F237B54_HPP)
