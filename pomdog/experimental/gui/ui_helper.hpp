// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/point2d.hpp"

namespace Pomdog::GUI::UIHelper {

[[nodiscard]] Point2D
ProjectToWorldSpace(const Point2D& positionInChild, const Point2D& toWorldSpace) noexcept;

[[nodiscard]] Point2D
ProjectToChildSpace(const Point2D& positionInWorld, const Point2D& toChildSpace) noexcept;

} // namespace Pomdog::GUI::UIHelper
