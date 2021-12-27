// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/point2d.hpp"

namespace pomdog::gui::UIHelper {

[[nodiscard]] Point2D
ProjectToWorldSpace(const Point2D& positionInChild, const Point2D& toWorldSpace) noexcept;

[[nodiscard]] Point2D
ProjectToChildSpace(const Point2D& positionInWorld, const Point2D& toChildSpace) noexcept;

} // namespace pomdog::gui::UIHelper
