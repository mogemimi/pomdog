//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_EDITORCOLORSCHEME_EFD082F5_C42B_4243_A6FB_292C08E782A4_HPP
#define POMDOG_EDITORCOLORSCHEME_EFD082F5_C42B_4243_A6FB_292C08E782A4_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <Pomdog/Math/Color.hpp>

namespace Pomdog {
namespace SceneEditor {

struct EditorColorScheme {
	Color Background {81, 81, 81, 255};
	Color CenterAxisX {255, 149, 140, 225};
	Color CenterAxisY {140, 220, 140, 225};
	Color CenterAxisZ {56, 190, 255, 225};
	Color Grid {154, 154, 153, 48};
	Color GuideLine {183, 182, 181, 102};
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // !defined(POMDOG_EDITORCOLORSCHEME_EFD082F5_C42B_4243_A6FB_292C08E782A4_HPP)
