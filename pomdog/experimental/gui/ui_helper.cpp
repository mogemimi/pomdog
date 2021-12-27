// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/ui_helper.h"

namespace pomdog::gui::UIHelper {

Point2D
ProjectToWorldSpace(const Point2D& positionInChild, const Point2D& toWorldSpace) noexcept
{
    return positionInChild + toWorldSpace;
}

Point2D
ProjectToChildSpace(const Point2D& positionInWorld, const Point2D& toChildSpace) noexcept
{
    return positionInWorld - toChildSpace;
}

} // namespace pomdog::gui::UIHelper
