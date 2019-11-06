// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <vector>

namespace Pomdog {

class LineBatch;

namespace SceneEditor {

class PrimitiveGrid {
public:
    PrimitiveGrid(const Color& primaryColor, const Color& secondaryColor);

    void Draw(LineBatch& lineBatch);

private:
    struct Line {
        Vector2 Point1;
        Vector2 Point2;
        Color Color;
    };

    std::vector<Line> lines;
};

} // namespace SceneEditor
} // namespace Pomdog
