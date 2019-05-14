// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Commands/PrimitivePolygonCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Rendering {

std::type_index PrimitivePolygonCommand::GetType() const noexcept
{
    static const std::type_index index = typeid(PrimitivePolygonCommand);
    return index;
}

void PrimitivePolygonArcCommand::Execute(PrimitiveBatch& primitiveBatch)
{
    primitiveBatch.DrawArc(position, radius, startAngle, arcAngle, segments, color);
}

void PrimitivePolylineCommand::Execute(PrimitiveBatch& primitiveBatch)
{
    primitiveBatch.DrawPolyline(points, thickness, color);
}

void PrimitiveFunctionCommand::Execute(PrimitiveBatch& primitiveBatch)
{
    POMDOG_ASSERT(invoker);
    if (invoker) {
        invoker(primitiveBatch);
    }
}

} // namespace Rendering
} // namespace Pomdog
