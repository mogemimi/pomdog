// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "BeamSystem.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include <algorithm>

namespace Pomdog {
namespace {

std::vector<Vector2> CreateJaggedLine(
    BeamEmitter const& emitter,
    std::uint32_t interpolationPoints,
    Vector2 const& start, Vector2 const& end, std::mt19937 & random)
{
    ///@todo Not implemented
    POMDOG_ASSERT_MESSAGE(Vector2::DistanceSquared(start, end) > std::numeric_limits<float>::epsilon(),
        "TODO: Not implemented");

    auto tangent = end - start;
    auto normal = Vector2{-tangent.Y, tangent.X};

    POMDOG_ASSERT(tangent.LengthSquared() > std::numeric_limits<float>::epsilon());
    POMDOG_ASSERT(normal.LengthSquared() > std::numeric_limits<float>::epsilon());

    std::vector<float> positions;

    for (size_t i = 0; i < interpolationPoints; ++i) {
        std::uniform_real_distribution<float> distribution(0, 1);
        positions.push_back(distribution(random));
    }

    std::sort(std::begin(positions), std::end(positions), std::less<float>());


    std::vector<Vector2> points;
    points.reserve(interpolationPoints + 2);
    points.push_back(start);

    Vector2 prevPoint = points.front();
    float prevDisplacement = 0;
    float prevPosition = 0;

    auto length = std::max(Vector2::Distance(start, end), std::numeric_limits<float>::epsilon());
    POMDOG_ASSERT(length > 0);

    for (auto & position: positions)
    {
        POMDOG_ASSERT(emitter.Jaggedness >= 0);
        float scale = position - prevPosition;
        scale = std::min((scale * interpolationPoints) * emitter.Jaggedness, 1.0f);
        scale = scale * scale;
        POMDOG_ASSERT(scale >= 0);

        std::uniform_real_distribution<float> distribution(emitter.SwayRange.a() / length, emitter.SwayRange.b() / length);

        POMDOG_ASSERT(!positions.empty());

        constexpr float threshold = 0.95f;
        constexpr float envelopeScale = (1.0f / (1.0f - threshold));
        float envelope = (position > threshold) ? envelopeScale * (1.0f - position): 1.0f;

        float displacement = MathHelper::SmoothStep(prevDisplacement, distribution(random), scale);
        displacement *= envelope;

        Vector2 point = start + tangent * position + normal * displacement;

        prevPoint = point;
        prevDisplacement = displacement;
        prevPosition = position;

        points.push_back(std::move(point));
    }

    points.push_back(end);

    POMDOG_ASSERT(!points.empty());
    POMDOG_ASSERT(points.size() >= 2);
    return points;
}

std::vector<Vector2> CreateBranch(BeamEmitter const& emitter,
    BeamBranching const& branching,
    Vector2 const& sourceStart, Vector2 const& sourceEnd, Beam const& parentBeam, std::mt19937 & random)
{
    POMDOG_ASSERT(!parentBeam.Points.empty());
    POMDOG_ASSERT(parentBeam.Points.size() >= 2);

    const std::uint32_t indexBegin = 0;
    const std::uint32_t indexEnd = static_cast<std::uint32_t>(parentBeam.Points.size()) - 1;

    std::uniform_int_distribution<std::uint32_t> indexDistribution(indexBegin, indexEnd);

    auto index1 = indexDistribution(random);
    auto index2 = index1 + 1;
    if (index1 >= indexEnd) {
        index2 = index1 - 1;
        std::swap(index1, index2);
    }
    POMDOG_ASSERT(index1 < parentBeam.Points.size());
    POMDOG_ASSERT(index2 < parentBeam.Points.size());

    std::uniform_real_distribution<float> middlePointDistribution(0.1f, 0.95f);
    auto middlePoint = middlePointDistribution(random);
    Vector2 start = Vector2::Lerp(parentBeam.Points[index1], parentBeam.Points[index2], middlePoint);

    Vector2 tangent = sourceEnd - start;
    Vector2 normal {-tangent.Y, tangent.X};

    auto distribution = branching.SpreadRange;
    auto lengthSq1 = (sourceEnd - start).LengthSquared();
    auto lengthSq2 = (sourceEnd - sourceStart).LengthSquared();
    POMDOG_ASSERT(lengthSq2 > std::numeric_limits<float>::epsilon());
    POMDOG_ASSERT(lengthSq1 > std::numeric_limits<float>::epsilon());

    auto amount = (lengthSq1 / (lengthSq2 * (middlePointDistribution.max() - middlePointDistribution.min())));

    Vector2 end = sourceEnd + (Vector2::Normalize(normal) * distribution(random) * amount);

    auto scale = Vector2::Distance(end, start) / Vector2::Distance(sourceEnd, sourceStart);

    return CreateJaggedLine(emitter, emitter.InterpolationPoints * scale, start, end, random);
}

} // unnamed namespace

BeamSystem::BeamSystem()
    : erapsedTime(0)
    , emissionTimer(0)
    , random(std::random_device{}())
{
    emitter.Looping = true;
    emitter.StartLifetime = 0.64f;
    emitter.SwayRange = std::uniform_real_distribution<float>{-9.0f, 9.0f};
    emitter.InterpolationPoints = 32;
    emitter.Jaggedness = 0.7f;
    emitter.MaxBeams = 100;
    emitter.EmissionRate = 10;
    emitter.StartColor = Color::White;
    emitter.EndColor = {255, 220, 130, 0};
    emitter.StartThickness = 1.4f;

    branching.InheritThickness = 0.4f;
    branching.MaxBranches = 10;
    branching.SpreadRange = std::uniform_real_distribution<float>{-40.0f, 40.0f};
    branching.BranchingRate = 0.9f;

    beams.reserve(emitter.MaxBeams);
}

void BeamSystem::Update(Duration const& frameDuration, Transform const& emitterTransform, Vector2 const& target)
{
    erapsedTime += frameDuration;

    if (emitter.Looping && erapsedTime > emitter.Duration) {
        erapsedTime = Duration::zero();
    }

    if (emitter.Looping || erapsedTime <= emitter.Duration)
    {
        emissionTimer += frameDuration;

        POMDOG_ASSERT(emitter.EmissionRate > 0);
        auto emissionInterval = std::max(std::numeric_limits<Duration>::epsilon(),
            Duration(1) / emitter.EmissionRate);

        POMDOG_ASSERT(emissionInterval.count() > 0);

        POMDOG_ASSERT(emitter.Duration.count() > 0);
        //float normalizedTime = erapsedTime / emitter.Duration;

        auto CreateBranchBeam = [this](std::size_t parentBeamIndex, Vector2 const& sourceStart, Vector2 const& sourceEnd)
        {
            std::uniform_real_distribution<float> distribution(0.0f, branching.MaxBranches * branching.BranchingRate);
            auto const maxBranches = distribution(random);

            for (std::size_t i = 0; i < maxBranches; ++i) {
                if (beams.size() + 1 >= emitter.MaxBeams) {
                    break;
                }

                auto & beam = beams[parentBeamIndex];

                Beam branchBeam;
                branchBeam.Points = CreateBranch(emitter, branching, sourceStart, sourceEnd, beam, random);
                branchBeam.TimeToLive = beam.TimeToLive;
                branchBeam.Color = beam.Color;
                branchBeam.Thickness = beam.Thickness * branching.InheritThickness;
                beams.push_back(std::move(branchBeam));

                if (branching.BranchingRate < std::generate_canonical<float, std::numeric_limits<float>::digits>(random))
                {
                    parentBeamIndex = beams.size() - 1;
                }
            }
        };

        auto distribution = std::uniform_real_distribution<float>(-emitter.ShapeWidth/2, emitter.ShapeWidth/2);

        while ((beams.size() < emitter.MaxBeams) && (emissionTimer >= emissionInterval))
        {
            emissionTimer -= emissionInterval;

            Vector2 tangent = target - emitterTransform.GetPosition2D();
            Vector2 normal {-tangent.Y, tangent.X};
            Vector2 end = target + (Vector2::Normalize(normal) * distribution(random));

            Beam beam;

            POMDOG_ASSERT(emitter.StartLifetime > 0.0f);
            beam.TimeToLive = emitter.StartLifetime;
            beam.Points = CreateJaggedLine(emitter, emitter.InterpolationPoints,
                emitterTransform.GetPosition2D(), end, random);
            beam.Color = emitter.StartColor;
            beam.Thickness = emitter.StartThickness;

            beams.push_back(std::move(beam));
            CreateBranchBeam(beams.size() - 1, emitterTransform.GetPosition2D(), target);
        }
    }
    {
        for (auto & beam: beams) {
            beam.TimeToLive -= frameDuration.count();
            if (beam.TimeToLive <= 0.0f) {
                continue;
            }

            float normalizedTime = 1.0f - beam.TimeToLive / emitter.StartLifetime;

            beam.Color = Color::SmoothStep(emitter.StartColor, emitter.EndColor, normalizedTime);
        }

        beams.erase(std::remove_if(std::begin(beams), std::end(beams),
            [](Beam const& beam){ return beam.TimeToLive <= 0; }), std::end(beams));
    }
}

Vector2 BeamSystem::CreateTarget(Transform const& emitterTransform, float distance)
{
    Vector2 tangent {
        distance * std::cos(emitterTransform.GetRotation2D().value),
        distance * std::sin(emitterTransform.GetRotation2D().value)};
    auto targetPosition = emitterTransform.GetPosition2D() + tangent;
    return targetPosition;
}

} // namespace Pomdog
