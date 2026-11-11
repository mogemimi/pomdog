// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/particles/particle_clip_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_box.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_cone.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_constant.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_curve.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_random.h"
#include "pomdog/experimental/particles/particle_clip.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/particles_clip_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <string_view>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

using pomdog::detail::particles::ParticleCurveKey;
using pomdog::detail::particles::ParticleEmitterShape;
using pomdog::detail::particles::ParticleEmitterShapeBox;
using pomdog::detail::particles::ParticleEmitterShapeCone;
using pomdog::detail::particles::ParticleEmitterShapeHemisphere;
using pomdog::detail::particles::ParticleEmitterShapeSector;
using pomdog::detail::particles::ParticleEmitterShapeSphere;
using pomdog::detail::particles::ParticleParameter;
using pomdog::detail::particles::ParticleParameterConstant;
using pomdog::detail::particles::ParticleParameterCurve;
using pomdog::detail::particles::ParticleParameterRandom;

// NOTE: schemas/particles_clip.fbs limits the number of particles, for which
// the particle system reserves memory.
constexpr i32 maxParticlesLimit = 4096;

[[nodiscard]] Color
toColor(const pomdogschemas::ParticlesColor& color) noexcept
{
    return Color{color.r(), color.g(), color.b(), color.a()};
}

/// Returns true if `value` is finite and not negative. Returns false for NaN.
[[nodiscard]] bool
isFiniteNonNegative(f32 value) noexcept
{
    return std::isfinite(value) && (value >= 0.0f);
}

/// Verifies that a curve has keys in ascending order of time within [0, 1],
/// as ParticleParameterCurve requires.
template <typename Key>
[[nodiscard]] std::unique_ptr<Error>
validateKeys(const flatbuffers::Vector<const Key*>* keys) noexcept
{
    if ((keys == nullptr) || (keys->size() == 0)) {
        return errors::make("curve has no keys");
    }

    f32 previousTime = 0.0f;
    for (const auto* key : *keys) {
        const auto time = key->time();
        if (!(time >= previousTime) || !(time <= 1.0f)) {
            return errors::make("keys must be in ascending order of time within [0, 1]");
        }
        previousTime = time;
    }
    return nullptr;
}

/// Creates a float parameter. `T` is f32 or Radian<f32>, which the file stores
/// as a float in radians.
template <typename T>
[[nodiscard]] std::tuple<std::unique_ptr<ParticleParameter<T>>, std::unique_ptr<Error>>
createFloatParameter(pomdogschemas::ParticlesFloatParameter type, const void* value)
{
    if (value == nullptr) {
        return std::make_tuple(nullptr, errors::make("parameter is missing"));
    }

    switch (type) {
    case pomdogschemas::ParticlesFloatParameter_ParticlesFloatConstant: {
        const auto* constant = static_cast<const pomdogschemas::ParticlesFloatConstant*>(value);
        if (!std::isfinite(constant->value())) {
            return std::make_tuple(nullptr, errors::make("constant is not finite"));
        }
        return std::make_tuple(std::make_unique<ParticleParameterConstant<T>>(T{constant->value()}), nullptr);
    }
    case pomdogschemas::ParticlesFloatParameter_ParticlesFloatRandom: {
        const auto* random = static_cast<const pomdogschemas::ParticlesFloatRandom*>(value);
        if (!std::isfinite(random->min()) || !std::isfinite(random->max())) {
            return std::make_tuple(nullptr, errors::make("random range is not finite"));
        }
        return std::make_tuple(std::make_unique<ParticleParameterRandom<T>>(T{random->min()}, T{random->max()}), nullptr);
    }
    case pomdogschemas::ParticlesFloatParameter_ParticlesFloatCurve: {
        const auto* curve = static_cast<const pomdogschemas::ParticlesFloatCurve*>(value);
        if (auto err = validateKeys(curve->keys()); err != nullptr) {
            return std::make_tuple(nullptr, std::move(err));
        }

        std::vector<ParticleCurveKey<T>> keys;
        keys.reserve(curve->keys()->size());
        for (const auto* key : *curve->keys()) {
            if (!std::isfinite(key->value())) {
                return std::make_tuple(nullptr, errors::make("curve has a value that is not finite"));
            }
            keys.push_back(ParticleCurveKey<T>{key->time(), T{key->value()}});
        }
        return std::make_tuple(std::make_unique<ParticleParameterCurve<T>>(std::move(keys)), nullptr);
    }
    default:
        break;
    }
    return std::make_tuple(nullptr, errors::make("parameter has an unknown type"));
}

[[nodiscard]] std::tuple<std::unique_ptr<ParticleParameter<Color>>, std::unique_ptr<Error>>
createColorParameter(pomdogschemas::ParticlesColorParameter type, const void* value)
{
    if (value == nullptr) {
        return std::make_tuple(nullptr, errors::make("parameter is missing"));
    }

    switch (type) {
    case pomdogschemas::ParticlesColorParameter_ParticlesColorConstant: {
        const auto* constant = static_cast<const pomdogschemas::ParticlesColorConstant*>(value);
        if (constant->value() == nullptr) {
            return std::make_tuple(nullptr, errors::make("constant has no color"));
        }
        return std::make_tuple(std::make_unique<ParticleParameterConstant<Color>>(toColor(*constant->value())), nullptr);
    }
    case pomdogschemas::ParticlesColorParameter_ParticlesColorRandom: {
        const auto* random = static_cast<const pomdogschemas::ParticlesColorRandom*>(value);
        if ((random->min() == nullptr) || (random->max() == nullptr)) {
            return std::make_tuple(nullptr, errors::make("random range has no colors"));
        }
        return std::make_tuple(std::make_unique<ParticleParameterRandom<Color>>(toColor(*random->min()), toColor(*random->max())), nullptr);
    }
    case pomdogschemas::ParticlesColorParameter_ParticlesColorCurve: {
        const auto* curve = static_cast<const pomdogschemas::ParticlesColorCurve*>(value);
        if (auto err = validateKeys(curve->keys()); err != nullptr) {
            return std::make_tuple(nullptr, std::move(err));
        }

        std::vector<ParticleCurveKey<Color>> keys;
        keys.reserve(curve->keys()->size());
        for (const auto* key : *curve->keys()) {
            keys.push_back(ParticleCurveKey<Color>{key->time(), toColor(key->value())});
        }
        return std::make_tuple(std::make_unique<ParticleParameterCurve<Color>>(std::move(keys)), nullptr);
    }
    default:
        break;
    }
    return std::make_tuple(nullptr, errors::make("parameter has an unknown type"));
}

[[nodiscard]] std::tuple<std::unique_ptr<ParticleEmitterShape>, std::unique_ptr<Error>>
createShape(const pomdogschemas::ParticlesClip& clip)
{
    if (clip.shape() == nullptr) {
        return std::make_tuple(nullptr, errors::make("shape is missing"));
    }

    switch (clip.shape_type()) {
    case pomdogschemas::ParticlesShape_ParticlesShapeBox: {
        const auto* box = clip.shape_as_ParticlesShapeBox();
        if (!isFiniteNonNegative(box->width()) || !isFiniteNonNegative(box->height()) || !isFiniteNonNegative(box->depth())) {
            return std::make_tuple(nullptr, errors::make("box has a negative size"));
        }
        return std::make_tuple(std::make_unique<ParticleEmitterShapeBox>(Vector3{box->width(), box->height(), box->depth()}), nullptr);
    }
    case pomdogschemas::ParticlesShape_ParticlesShapeCone: {
        const auto* cone = clip.shape_as_ParticlesShapeCone();
        if (!isFiniteNonNegative(cone->radius()) || !isFiniteNonNegative(cone->height())) {
            return std::make_tuple(nullptr, errors::make("cone has a negative size"));
        }
        return std::make_tuple(std::make_unique<ParticleEmitterShapeCone>(cone->radius(), cone->height()), nullptr);
    }
    case pomdogschemas::ParticlesShape_ParticlesShapeHemisphere: {
        const auto* hemisphere = clip.shape_as_ParticlesShapeHemisphere();
        if (!isFiniteNonNegative(hemisphere->radius())) {
            return std::make_tuple(nullptr, errors::make("hemisphere has a negative radius"));
        }
        return std::make_tuple(std::make_unique<ParticleEmitterShapeHemisphere>(hemisphere->radius()), nullptr);
    }
    case pomdogschemas::ParticlesShape_ParticlesShapeSector: {
        const auto* sector = clip.shape_as_ParticlesShapeSector();
        if (!isFiniteNonNegative(sector->angle()) || (sector->angle() > math::TwoPi<f32>)) {
            return std::make_tuple(nullptr, errors::make("sector angle must be within [0, 2pi]"));
        }
        return std::make_tuple(std::make_unique<ParticleEmitterShapeSector>(Radian<f32>{sector->angle()}), nullptr);
    }
    case pomdogschemas::ParticlesShape_ParticlesShapeSphere: {
        const auto* sphere = clip.shape_as_ParticlesShapeSphere();
        if (!isFiniteNonNegative(sphere->radius())) {
            return std::make_tuple(nullptr, errors::make("sphere has a negative radius"));
        }
        return std::make_tuple(std::make_unique<ParticleEmitterShapeSphere>(sphere->radius()), nullptr);
    }
    default:
        break;
    }
    return std::make_tuple(nullptr, errors::make("shape has an unknown type"));
}

template <typename T>
[[nodiscard]] std::unique_ptr<Error>
setFloat(
    std::unique_ptr<ParticleParameter<T>>& target,
    pomdogschemas::ParticlesFloatParameter type,
    const void* value,
    std::string_view name)
{
    auto [parameter, err] = createFloatParameter<T>(type, value);
    if (err != nullptr) {
        return errors::wrap(std::move(err), pomdog::format("invalid {}", name));
    }
    target = std::move(parameter);
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
setColor(
    std::unique_ptr<ParticleParameter<Color>>& target,
    pomdogschemas::ParticlesColorParameter type,
    const void* value,
    std::string_view name)
{
    auto [parameter, err] = createColorParameter(type, value);
    if (err != nullptr) {
        return errors::wrap(std::move(err), pomdog::format("invalid {}", name));
    }
    target = std::move(parameter);
    return nullptr;
}

[[nodiscard]] std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
createParticleClip(const pomdogschemas::ParticlesClip& source)
{
    // NOTE: ParticleSystem relies on these ranges.
    if (!std::isfinite(source.duration()) || !(source.duration() > 0.0)) {
        return std::make_tuple(nullptr, errors::make("duration must be greater than zero"));
    }
    if ((source.emission_rate() <= 0) || (source.emission_rate_over_time() <= 0)) {
        return std::make_tuple(nullptr, errors::make("emission rates must be greater than zero"));
    }
    if ((source.max_particles() <= 0) || (source.max_particles() > maxParticlesLimit)) {
        return std::make_tuple(nullptr, errors::make(pomdog::format("max particles must be within [1, {}]", maxParticlesLimit)));
    }
    if (!std::isfinite(source.start_lifetime()) || !(source.start_lifetime() > 0.0f)) {
        return std::make_tuple(nullptr, errors::make("start lifetime must be greater than zero"));
    }
    if (!std::isfinite(source.start_delay()) || !std::isfinite(source.gravity_modifier())) {
        return std::make_tuple(nullptr, errors::make("start delay and gravity modifier must be finite"));
    }

    auto clip = std::make_shared<ParticleClip>();
    clip->Duration = Duration{source.duration()};
    clip->EmissionRate = source.emission_rate();
    clip->EmissionRateOverTime = source.emission_rate_over_time();
    clip->MaxParticles = source.max_particles();
    clip->StartLifetime = source.start_lifetime();
    clip->StartDelay = source.start_delay();
    clip->GravityModifier = source.gravity_modifier();
    clip->Looping = source.looping();

    if (auto [shape, err] = createShape(source); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    else {
        clip->Shape = std::move(shape);
    }

    if (auto err = setFloat(clip->StartSpeed, source.start_speed_type(), source.start_speed(), "start speed"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = setFloat(clip->StartRotation, source.start_rotation_type(), source.start_rotation(), "start rotation"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = setFloat(clip->RotationOverLifetime, source.rotation_over_lifetime_type(), source.rotation_over_lifetime(), "rotation over lifetime"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = setFloat(clip->StartSize, source.start_size_type(), source.start_size(), "start size"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = setFloat(clip->SizeOverLifetime, source.size_over_lifetime_type(), source.size_over_lifetime(), "size over lifetime"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = setColor(clip->StartColor, source.start_color_type(), source.start_color(), "start color"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = setColor(clip->ColorOverLifetime, source.color_over_lifetime_type(), source.color_over_lifetime(), "color over lifetime"); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(clip), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
loadParticleClip(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath)
{
    auto [file, openErr] = vfs::open(fs, filePath);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "cannot open particle clip file, " + filePath));
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(viewErr), "cannot read particle clip file, " + filePath));
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifyParticlesClipBuffer(verifier)) {
        return std::make_tuple(nullptr, errors::make("FlatBuffers verification failed for particle clip file, " + filePath));
    }

    auto [clip, err] = createParticleClip(*pomdogschemas::GetParticlesClip(bytes.data()));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "invalid particle clip file, " + filePath));
    }
    return std::make_tuple(std::move(clip), nullptr);
}

} // namespace pomdog
