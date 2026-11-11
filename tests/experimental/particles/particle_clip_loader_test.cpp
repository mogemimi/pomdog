// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"

// FIXME: Suppress warnings for the ParticleClip class, whose copy operations
// are implicitly deleted.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
#include "pomdog/experimental/particles/particle_clip.h"
POMDOG_MSVC_SUPPRESS_WARNING_POP

#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_box.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_cone.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_hemisphere.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sphere.h"
#include "pomdog/experimental/particles/particle_clip_loader.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/particles_clip_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::Color;
using pomdog::Error;
using pomdog::f32;
using pomdog::ParticleClip;
using pomdog::u8;
using pomdog::detail::particles::ParticleEmitterShapeBox;
using pomdog::detail::particles::ParticleEmitterShapeCone;
using pomdog::detail::particles::ParticleEmitterShapeHemisphere;
using pomdog::detail::particles::ParticleEmitterShapeSector;
using pomdog::detail::particles::ParticleEmitterShapeSphere;

namespace {

/// TemporaryFile writes bytes to a file in the temporary directory, and removes
/// the file when it goes out of scope.
class TemporaryFile final {
private:
    std::filesystem::path path_;
    std::string directory_;
    std::string name_;

public:
    explicit TemporaryFile(std::span<const u8> bytes)
    {
        // NOTE: Give every file its own name, so that tests do not share one.
        static int serial = 0;
        name_ = "pomdog_particle_clip_loader_test_" + std::to_string(serial) + ".particlesclip";
        ++serial;

        auto [tempDir, tempErr] = pomdog::platformfs::getTempDirectoryPath();
        REQUIRE(tempErr == nullptr);
        directory_ = tempDir;
        path_ = std::filesystem::path{tempDir} / name_;

        std::ofstream stream(path_, std::ios::binary | std::ios::trunc);
        REQUIRE(stream.is_open());
        stream.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        REQUIRE(stream.good());
    }

    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;
    TemporaryFile(TemporaryFile&&) = delete;
    TemporaryFile& operator=(TemporaryFile&&) = delete;

    ~TemporaryFile()
    {
        std::error_code err;
        std::filesystem::remove(path_, err);
    }

    /// Loads the file as a particle clip through a VFS that mounts the
    /// temporary directory.
    [[nodiscard]] std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
    load() const
    {
        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);
        REQUIRE(pomdog::vfs::mount(ctx, "/tmp", directory_, {.readOnly = true}) == nullptr);
        return pomdog::loadParticleClip(ctx, "/tmp/" + name_);
    }
};

[[nodiscard]] std::vector<u8>
pack(const pomdogschemas::ParticlesClipT& clip)
{
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(pomdogschemas::ParticlesClip::Pack(builder, &clip));

    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto bytes = std::span<const u8>{builder.GetBufferPointer(), builder.GetSize()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
    return std::vector<u8>(bytes.begin(), bytes.end());
}

[[nodiscard]] std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
loadBytes(std::span<const u8> bytes)
{
    const TemporaryFile file{bytes};
    return file.load();
}

[[nodiscard]] std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
load(const pomdogschemas::ParticlesClipT& clip)
{
    return loadBytes(pack(clip));
}

[[nodiscard]] pomdogschemas::ParticlesFloatParameterUnion
makeConstant(f32 value)
{
    pomdogschemas::ParticlesFloatParameterUnion parameter;
    auto constant = pomdogschemas::ParticlesFloatConstantT{};
    constant.value = value;
    parameter.Set(std::move(constant));
    return parameter;
}

[[nodiscard]] pomdogschemas::ParticlesFloatParameterUnion
makeRandom(f32 min, f32 max)
{
    pomdogschemas::ParticlesFloatParameterUnion parameter;
    auto random = pomdogschemas::ParticlesFloatRandomT{};
    random.min = min;
    random.max = max;
    parameter.Set(std::move(random));
    return parameter;
}

[[nodiscard]] pomdogschemas::ParticlesFloatParameterUnion
makeCurve(std::vector<pomdogschemas::ParticlesFloatKey> keys)
{
    pomdogschemas::ParticlesFloatParameterUnion parameter;
    auto curve = pomdogschemas::ParticlesFloatCurveT{};
    curve.keys = std::move(keys);
    parameter.Set(std::move(curve));
    return parameter;
}

/// Returns a clip that emits from a box, and has a parameter of each type:
/// constants, random ranges, and curves.
[[nodiscard]] pomdogschemas::ParticlesClipT
makeClip()
{
    pomdogschemas::ParticlesClipT clip;
    clip.duration = 0.5;
    clip.emission_rate = 450;
    clip.emission_rate_over_time = 100;
    clip.max_particles = 1024;
    clip.start_lifetime = 1.8f;
    clip.start_delay = 0.25f;
    clip.gravity_modifier = 980.0f;
    clip.looping = true;

    auto box = pomdogschemas::ParticlesShapeBoxT{};
    box.width = 20.0f;
    box.height = 50.0f;
    box.depth = 2.0f;
    clip.shape.Set(std::move(box));

    clip.start_speed = makeRandom(100.0f, 500.0f);
    clip.start_rotation = makeRandom(0.0f, pomdog::math::Pi<f32>);
    clip.rotation_over_lifetime = makeConstant(0.5f);
    clip.start_size = makeConstant(0.25f);
    clip.size_over_lifetime = makeCurve({{0.0f, 0.0f}, {0.5f, 1.0f}, {1.0f, 0.0f}});

    auto startColor = pomdogschemas::ParticlesColorRandomT{};
    startColor.min = std::make_unique<pomdogschemas::ParticlesColor>(u8(0), u8(0), u8(0), u8(255));
    startColor.max = std::make_unique<pomdogschemas::ParticlesColor>(u8(200), u8(180), u8(100), u8(50));
    clip.start_color.Set(std::move(startColor));

    auto colorOverLifetime = pomdogschemas::ParticlesColorCurveT{};
    colorOverLifetime.keys = {
        {0.0f, pomdogschemas::ParticlesColor{u8(100), u8(0), u8(0), u8(0)}},
        {1.0f, pomdogschemas::ParticlesColor{u8(0), u8(100), u8(0), u8(200)}},
    };
    clip.color_over_lifetime.Set(std::move(colorOverLifetime));

    return clip;
}

} // namespace

TEST_CASE("loadParticleClip")
{
    SUBCASE("loads the fields of a clip")
    {
        auto [clip, err] = load(makeClip());
        REQUIRE(err == nullptr);
        REQUIRE(clip != nullptr);

        REQUIRE(clip->Duration.count() == doctest::Approx(0.5));
        REQUIRE(clip->EmissionRate == 450);
        REQUIRE(clip->EmissionRateOverTime == 100);
        REQUIRE(clip->MaxParticles == 1024);
        REQUIRE(clip->StartLifetime == doctest::Approx(1.8f));
        REQUIRE(clip->StartDelay == doctest::Approx(0.25f));
        REQUIRE(clip->GravityModifier == doctest::Approx(980.0f));
        REQUIRE(clip->Looping);
    }
    SUBCASE("creates the parameters")
    {
        auto [clip, err] = load(makeClip());
        REQUIRE(err == nullptr);

        // NOTE: A random parameter interpolates between its range by the amount.
        REQUIRE(clip->StartSpeed->Compute(0.0f, 0.0f) == doctest::Approx(100.0f));
        REQUIRE(clip->StartSpeed->Compute(0.0f, 1.0f) == doctest::Approx(500.0f));
        REQUIRE(clip->StartRotation->Compute(0.0f, 0.5f).get() == doctest::Approx(pomdog::math::PiOver2<f32>));

        // NOTE: A constant ignores the time and the amount.
        REQUIRE(clip->RotationOverLifetime->Compute(0.3f, 0.7f).get() == doctest::Approx(0.5f));
        REQUIRE(clip->StartSize->Compute(0.3f, 0.7f) == doctest::Approx(0.25f));

        // NOTE: A curve interpolates between its keys by the time.
        REQUIRE(clip->SizeOverLifetime->Compute(0.25f, 0.0f) == doctest::Approx(0.5f));
        REQUIRE(clip->SizeOverLifetime->Compute(0.5f, 0.0f) == doctest::Approx(1.0f));
        REQUIRE(clip->SizeOverLifetime->Compute(0.75f, 0.0f) == doctest::Approx(0.5f));

        REQUIRE(clip->StartColor->Compute(0.0f, 0.0f) == Color{0, 0, 0, 255});
        REQUIRE(clip->StartColor->Compute(0.0f, 1.0f) == Color{200, 180, 100, 50});
        REQUIRE(clip->ColorOverLifetime->Compute(0.0f, 0.0f) == Color{100, 0, 0, 0});
        REQUIRE(clip->ColorOverLifetime->Compute(1.0f, 0.0f) == Color{0, 100, 0, 200});
    }
    SUBCASE("creates each shape")
    {
        auto clip = makeClip();

        REQUIRE(dynamic_cast<const ParticleEmitterShapeBox*>(std::get<0>(load(clip))->Shape.get()) != nullptr);

        auto cone = pomdogschemas::ParticlesShapeConeT{};
        cone.radius = 0.5f;
        cone.height = 1.0f;
        clip.shape.Set(std::move(cone));
        REQUIRE(dynamic_cast<const ParticleEmitterShapeCone*>(std::get<0>(load(clip))->Shape.get()) != nullptr);

        auto hemisphere = pomdogschemas::ParticlesShapeHemisphereT{};
        hemisphere.radius = 1.0f;
        clip.shape.Set(std::move(hemisphere));
        REQUIRE(dynamic_cast<const ParticleEmitterShapeHemisphere*>(std::get<0>(load(clip))->Shape.get()) != nullptr);

        auto sector = pomdogschemas::ParticlesShapeSectorT{};
        sector.angle = pomdog::math::TwoPi<f32>;
        clip.shape.Set(std::move(sector));
        REQUIRE(dynamic_cast<const ParticleEmitterShapeSector*>(std::get<0>(load(clip))->Shape.get()) != nullptr);

        auto sphere = pomdogschemas::ParticlesShapeSphereT{};
        sphere.radius = 1.0f;
        clip.shape.Set(std::move(sphere));
        REQUIRE(dynamic_cast<const ParticleEmitterShapeSphere*>(std::get<0>(load(clip))->Shape.get()) != nullptr);
    }
    SUBCASE("fails for a missing file")
    {
        auto [ctx, ctxErr] = pomdog::vfs::create();
        REQUIRE(ctxErr == nullptr);

        auto [clip, err] = pomdog::loadParticleClip(ctx, "/tmp/pomdog_particle_clip_loader_test_missing.particlesclip");
        REQUIRE(clip == nullptr);
        REQUIRE(err != nullptr);
    }
    SUBCASE("rejects truncated bytes")
    {
        auto bytes = pack(makeClip());
        bytes.resize(bytes.size() / 2);

        auto [clip, err] = loadBytes(bytes);
        REQUIRE(clip == nullptr);
        REQUIRE(err != nullptr);
    }
}

TEST_CASE("loadParticleClip rejects invalid clips")
{
    // NOTE: Each case breaks one rule of schemas/particles_clip.fbs.
    const auto requireRejected = [](const pomdogschemas::ParticlesClipT& clip) {
        auto [loaded, err] = load(clip);
        REQUIRE(loaded == nullptr);
        REQUIRE(err != nullptr);
    };

    SUBCASE("duration of zero")
    {
        auto clip = makeClip();
        clip.duration = 0.0;
        requireRejected(clip);
    }
    SUBCASE("emission rate of zero")
    {
        auto clip = makeClip();
        clip.emission_rate = 0;
        requireRejected(clip);
    }
    SUBCASE("negative emission rate over time")
    {
        auto clip = makeClip();
        clip.emission_rate_over_time = -1;
        requireRejected(clip);
    }
    SUBCASE("no particles")
    {
        auto clip = makeClip();
        clip.max_particles = 0;
        requireRejected(clip);
    }
    SUBCASE("too many particles")
    {
        auto clip = makeClip();
        clip.max_particles = 4097;
        requireRejected(clip);
    }
    SUBCASE("lifetime of zero")
    {
        auto clip = makeClip();
        clip.start_lifetime = 0.0f;
        requireRejected(clip);
    }
    SUBCASE("gravity that is not a number")
    {
        auto clip = makeClip();
        clip.gravity_modifier = std::numeric_limits<f32>::quiet_NaN();
        requireRejected(clip);
    }
    SUBCASE("missing shape")
    {
        auto clip = makeClip();
        clip.shape.Reset();
        requireRejected(clip);
    }
    SUBCASE("negative box size")
    {
        auto clip = makeClip();
        clip.shape.AsParticlesShapeBox()->depth = -1.0f;
        requireRejected(clip);
    }
    SUBCASE("sector angle over a turn")
    {
        auto clip = makeClip();
        auto sector = pomdogschemas::ParticlesShapeSectorT{};
        sector.angle = 7.0f;
        clip.shape.Set(std::move(sector));
        requireRejected(clip);
    }
    SUBCASE("missing parameter")
    {
        auto clip = makeClip();
        clip.start_size.Reset();
        requireRejected(clip);
    }
    SUBCASE("missing color")
    {
        auto clip = makeClip();
        clip.start_color.AsParticlesColorRandom()->max.reset();
        requireRejected(clip);
    }
    SUBCASE("random range that is not finite")
    {
        auto clip = makeClip();
        clip.start_speed = makeRandom(0.0f, std::numeric_limits<f32>::infinity());
        requireRejected(clip);
    }
    SUBCASE("curve without keys")
    {
        auto clip = makeClip();
        clip.size_over_lifetime = makeCurve({});
        requireRejected(clip);
    }
    SUBCASE("keys that are not in ascending order")
    {
        auto clip = makeClip();
        clip.size_over_lifetime = makeCurve({{0.5f, 1.0f}, {0.0f, 0.0f}});
        requireRejected(clip);
    }
    SUBCASE("key out of the lifetime")
    {
        auto clip = makeClip();
        clip.size_over_lifetime = makeCurve({{0.0f, 0.0f}, {1.5f, 1.0f}});
        requireRejected(clip);
    }
    SUBCASE("color keys that are not in ascending order")
    {
        auto clip = makeClip();
        auto curve = pomdogschemas::ParticlesColorCurveT{};
        curve.keys = {
            {1.0f, pomdogschemas::ParticlesColor{u8(0), u8(0), u8(0), u8(0)}},
            {0.0f, pomdogschemas::ParticlesColor{u8(0), u8(0), u8(0), u8(0)}},
        };
        clip.color_over_lifetime.Set(std::move(curve));
        requireRejected(clip);
    }
}
