// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/GUI/UIHelper.hpp"

namespace Pomdog::GUI::UIHelper {

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

} // namespace Pomdog::GUI::UIHelper
