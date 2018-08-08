// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog/Experimental/Graphics/PolygonBatch.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Rendering {

class PrimitivePolygonCommand : public RenderCommand {
public:
    virtual ~PrimitivePolygonCommand() = default;

    virtual void Execute(PolygonBatch & spriteBatch) = 0;

    std::type_index GetType() const noexcept override final;
};

class PrimitivePolygonArcCommand final : public PrimitivePolygonCommand {
public:
    PrimitivePolygonArcCommand()
        : position(Vector2::Zero)
        , radius(1.0f)
        , startAngle(0.0f)
        , arcAngle(0.0f)
        , segments(3)
        , color(Color::White)
    {
    }

    void SetArc(
        const Radian<float>& startAngleIn,
        const Radian<float>& arcAngleIn)
    {
        startAngle = startAngleIn;
        arcAngle = arcAngleIn;
    }

    void SetSegments(int segmentsIn) noexcept
    {
        POMDOG_ASSERT(segmentsIn >= 3);
        segments = segmentsIn;
    }

    void SetRadius(float radiusIn) noexcept
    {
        POMDOG_ASSERT(radiusIn >= 0.0f);
        radius = radiusIn;
    }

    void SetPosition(const Vector2& positionIn) noexcept
    {
        position = positionIn;
    }

    void SetColor(const Color& colorIn) noexcept
    {
        color = colorIn;
    }

    void Execute(PolygonBatch & polygonBatch) override;

private:
    Vector2 position;
    float radius;
    Radian<float> startAngle;
    Radian<float> arcAngle;
    int segments;
    Color color;
};

class PrimitivePolylineCommand final : public PrimitivePolygonCommand {
public:
    PrimitivePolylineCommand()
        : thickness(1.0f)
        , color(Color::White)
    {
    }

    void SetPoints(const std::vector<Vector2>& pointsIn)
    {
        points = pointsIn;
    }

    void AddPoint(const Vector2& point)
    {
        points.push_back(point);
    }

    void SetThickness(float thicknessIn) noexcept
    {
        POMDOG_ASSERT(thicknessIn >= 0.0f);
        thickness = thicknessIn;
    }

    void SetColor(const Color& colorIn) noexcept
    {
        color = colorIn;
    }

    void Execute(PolygonBatch & polygonBatch) override;

private:
    std::vector<Vector2> points;
    float thickness;
    Color color;
};

class PrimitiveFunctionCommand final : public PrimitivePolygonCommand {
public:
    PrimitiveFunctionCommand()
    {
    }

    void SetInvoker(std::function<void(PolygonBatch &)> && invokerIn)
    {
        POMDOG_ASSERT(invokerIn);
        invoker = std::move(invokerIn);
    }

    void Execute(PolygonBatch & polygonBatch) override;

private:
    std::function<void(PolygonBatch & polygonBatch)> invoker;
};

} // namespace Rendering
} // namespace Pomdog
