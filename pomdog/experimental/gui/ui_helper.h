// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/point2d.h"

namespace pomdog::gui::UIHelper {

[[nodiscard]] Point2D
projectToWorldSpace(const Point2D& positionInChild, const Point2D& toWorldSpace) noexcept;

[[nodiscard]] Point2D
projectToChildSpace(const Point2D& positionInWorld, const Point2D& toChildSpace) noexcept;

} // namespace pomdog::gui::UIHelper
