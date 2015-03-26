// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EDITORCOLORSCHEME_EFD082F5_HPP
#define POMDOG_EDITORCOLORSCHEME_EFD082F5_HPP

#include "Pomdog/Math/Color.hpp"

namespace Pomdog {
namespace SceneEditor {

struct EditorColorScheme {
    Color Background {81, 81, 81, 255};
    Color BackgroundDark {51, 51, 51, 255};
    Color BackgroundGradientUpper {90, 86, 80, 255};
    Color BackgroundGradientLower {57, 67, 86, 255};
    Color CenterAxisX {255, 149, 140, 225};
    Color CenterAxisY {140, 220, 140, 225};
    Color CenterAxisZ {56, 190, 255, 225};
    Color Grid {154, 154, 153, 48};
    Color GuideLine {183, 182, 181, 102};
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // POMDOG_EDITORCOLORSCHEME_EFD082F5_HPP
