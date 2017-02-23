// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Rendering {

std::type_index PrimitivePolygonCommand::GetType() const noexcept
{
    static const std::type_index index = typeid(PrimitivePolygonCommand);
    return index;
}

void PrimitivePolygonArcCommand::Execute(PolygonBatch & polygonBatch)
{
    polygonBatch.DrawArc(position, radius, startAngle, arcAngle, segments, color);
}

void PrimitivePolylineCommand::Execute(PolygonBatch & polygonBatch)
{
    polygonBatch.DrawPolyline(points, thickness, color);
}

void PrimitiveFunctionCommand::Execute(PolygonBatch & polygonBatch)
{
    POMDOG_ASSERT(invoker);
    if (invoker) {
        invoker(polygonBatch);
    }
}

} // namespace Rendering
} // namespace Pomdog
