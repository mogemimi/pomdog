// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/beam_system.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/radian.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

std::vector<Vector2>
createJaggedLine(
    const BeamEmitter& emitter,
    u32 interpolationPoints,
    const Vector2& start,
    const Vector2& end,
    random::Xoroshiro128StarStar& random)
{
    if (math::distanceSquared(start, end) <= std::numeric_limits<f32>::epsilon()) {
        return {};
    }

    auto tangent = end - start;
    auto normal = Vector2{-tangent.y, tangent.x};

    POMDOG_ASSERT(math::lengthSquared(tangent) > std::numeric_limits<f32>::epsilon());
    POMDOG_ASSERT(math::lengthSquared(normal) > std::numeric_limits<f32>::epsilon());

    std::vector<f32> positions;

    for (size_t i = 0; i < interpolationPoints; ++i) {
        std::uniform_real_distribution<f32> distribution(0, 1);
        positions.push_back(distribution(random));
    }

    std::sort(std::begin(positions), std::end(positions), std::less<f32>());

    std::vector<Vector2> points;
    points.reserve(interpolationPoints + 2);
    points.push_back(start);

    Vector2 prevPoint = points.front();
    f32 prevDisplacement = 0;
    f32 prevPosition = 0;

    auto length = std::max(math::distance(start, end), std::numeric_limits<f32>::epsilon());
    POMDOG_ASSERT(length > 0);

    for (auto& position : positions) {
        POMDOG_ASSERT(emitter.jaggedness >= 0);
        f32 scale = position - prevPosition;
        scale = std::min((scale * interpolationPoints) * emitter.jaggedness, 1.0f);
        scale = scale * scale;
        POMDOG_ASSERT(scale >= 0);

        std::uniform_real_distribution<f32> distribution(emitter.swayRange.a() / length, emitter.swayRange.b() / length);

        POMDOG_ASSERT(!positions.empty());

        constexpr f32 threshold = 0.95f;
        constexpr f32 envelopeScale = (1.0f / (1.0f - threshold));
        f32 envelope = (position > threshold) ? envelopeScale * (1.0f - position) : 1.0f;

        f32 displacement = math::smoothstep(prevDisplacement, distribution(random), scale);
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

std::vector<Vector2>
createBranch(
    const BeamEmitter& emitter,
    const BeamBranching& branching,
    const Vector2& sourceStart,
    const Vector2& sourceEnd,
    const Beam& parentBeam,
    random::Xoroshiro128StarStar& random)
{
    POMDOG_ASSERT(!parentBeam.points.empty());
    POMDOG_ASSERT(parentBeam.points.size() >= 2);

    const u32 indexBegin = 0;
    const u32 indexEnd = static_cast<u32>(parentBeam.points.size()) - 1;

    std::uniform_int_distribution<u32> indexDistribution(indexBegin, indexEnd);

    auto index1 = indexDistribution(random);
    auto index2 = index1 + 1;
    if (index1 >= indexEnd) {
        index2 = index1 - 1;
        std::swap(index1, index2);
    }
    POMDOG_ASSERT(index1 < parentBeam.points.size());
    POMDOG_ASSERT(index2 < parentBeam.points.size());

    std::uniform_real_distribution<f32> middlePointDistribution(0.1f, 0.95f);
    const auto middlePoint = middlePointDistribution(random);
    const auto start = math::lerp(parentBeam.points[index1], parentBeam.points[index2], middlePoint);

    const auto tangent = sourceEnd - start;
    const auto normal = Vector2{-tangent.y, tangent.x};

    auto distribution = branching.spreadRange;
    const auto lengthSq1 = math::lengthSquared(sourceEnd - start);
    const auto lengthSq2 = math::lengthSquared(sourceEnd - sourceStart);
    POMDOG_ASSERT(lengthSq2 > std::numeric_limits<f32>::epsilon());
    POMDOG_ASSERT(lengthSq1 > std::numeric_limits<f32>::epsilon());

    auto amount = (lengthSq1 / (lengthSq2 * (middlePointDistribution.max() - middlePointDistribution.min())));

    Vector2 end = sourceEnd + (math::normalize(normal) * distribution(random) * amount);

    auto scale = math::distance(end, start) / math::distance(sourceEnd, sourceStart);
    auto scaledPoints = static_cast<u32>(emitter.interpolationPoints * scale);

    return createJaggedLine(emitter, scaledPoints, start, end, random);
}

} // namespace

BeamSystem::BeamSystem()
    : elapsedTime_(0)
    , emissionTimer_(0)
    , random_(std::random_device{}())
{
    emitter.looping = true;
    emitter.startLifetime = 0.64f;
    emitter.swayRange = std::uniform_real_distribution<f32>{-9.0f, 9.0f};
    emitter.interpolationPoints = 32;
    emitter.jaggedness = 0.7f;
    emitter.maxBeams = 100;
    emitter.emissionRate = 10;
    emitter.startColor = Color::createWhite();
    emitter.endColor = {255, 220, 130, 0};
    emitter.startThickness = 1.4f;

    branching.inheritThickness = 0.4f;
    branching.maxBranches = 10;
    branching.spreadRange = std::uniform_real_distribution<f32>{-40.0f, 40.0f};
    branching.branchingRate = 0.9f;

    beams.reserve(emitter.maxBeams);
}

void BeamSystem::update(const Duration& frameDuration, const Vector2& emitterPosition, const Vector2& target)
{
    elapsedTime_ += frameDuration;

    if (emitter.looping && elapsedTime_ > emitter.duration) {
        elapsedTime_ = Duration::zero();
    }

    if (emitter.looping || elapsedTime_ <= emitter.duration) {
        emissionTimer_ += frameDuration;

        POMDOG_ASSERT(emitter.emissionRate > 0);
        auto emissionInterval = std::max(std::numeric_limits<Duration>::epsilon(),
            Duration(1) / emitter.emissionRate);

        POMDOG_ASSERT(emissionInterval.count() > 0);

        POMDOG_ASSERT(emitter.duration.count() > 0);
        //f32 normalizedTime = elapsedTime_ / emitter.duration;

        auto createBranchBeam = [this](std::size_t parentBeamIndex, const Vector2& sourceStart, const Vector2& sourceEnd) {
            std::uniform_real_distribution<f32> distribution(0.0f, branching.maxBranches * branching.branchingRate);
            auto const maxBranches = distribution(random_);

            for (std::size_t i = 0; i < maxBranches; ++i) {
                if (beams.size() + 1 >= emitter.maxBeams) {
                    break;
                }

                auto& beam = beams[parentBeamIndex];

                Beam branchBeam;
                branchBeam.points = createBranch(emitter, branching, sourceStart, sourceEnd, beam, random_);
                branchBeam.timeToLive = beam.timeToLive;
                branchBeam.color = beam.color;
                branchBeam.thickness = beam.thickness * branching.inheritThickness;
                beams.push_back(std::move(branchBeam));

                if (branching.branchingRate < std::generate_canonical<f32, std::numeric_limits<f32>::digits>(random_)) {
                    parentBeamIndex = beams.size() - 1;
                }
            }
        };

        auto distribution = std::uniform_real_distribution<f32>(-emitter.shapeWidth / 2, emitter.shapeWidth / 2);

        while ((beams.size() < emitter.maxBeams) && (emissionTimer_ >= emissionInterval)) {
            emissionTimer_ -= emissionInterval;

            const auto tangent = target - emitterPosition;
            const auto normal = Vector2{-tangent.y, tangent.x};
            const auto end = target + (math::normalize(normal) * distribution(random_));

            Beam beam;

            POMDOG_ASSERT(emitter.startLifetime > 0.0f);
            beam.timeToLive = emitter.startLifetime;
            beam.points = createJaggedLine(
                emitter, emitter.interpolationPoints, emitterPosition, end, random_);
            beam.color = emitter.startColor;
            beam.thickness = emitter.startThickness;

            if (beam.points.empty()) {
                continue;
            }
            beams.push_back(std::move(beam));
            createBranchBeam(beams.size() - 1, emitterPosition, target);
        }
    }
    {
        for (auto& beam : beams) {
            beam.timeToLive -= static_cast<f32>(frameDuration.count());
            if (beam.timeToLive <= 0.0f) {
                continue;
            }

            f32 normalizedTime = 1.0f - beam.timeToLive / emitter.startLifetime;

            beam.color = math::smoothstep(emitter.startColor, emitter.endColor, normalizedTime);
        }

        beams.erase(
            std::remove_if(
                std::begin(beams),
                std::end(beams),
                [](const Beam& beam) { return beam.timeToLive <= 0; }),
            std::end(beams));
    }
}

Vector2 BeamSystem::createTarget(
    const Vector2& emitterPosition,
    const Radian<f32>& emitterRotation,
    f32 distance)
{
    auto tangent = Vector2{
        distance * std::cos(emitterRotation.get()),
        distance * std::sin(emitterRotation.get()),
    };
    auto targetPosition = emitterPosition + tangent;
    return targetPosition;
}

} // namespace pomdog
