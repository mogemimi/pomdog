// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>

namespace Pomdog {

class LineBatch;

namespace SceneEditor {

class PrimitiveGrid {
public:
    PrimitiveGrid(Color const& primaryColor, Color const& secondaryColor);

    void Draw(LineBatch & lineBatch);

private:
    struct Line {
        Vector2 Point1;
        Vector2 Point2;
        Color Color;
    };

    std::vector<Line> lines;
};

}// namespace SceneEditor
}// namespace Pomdog
