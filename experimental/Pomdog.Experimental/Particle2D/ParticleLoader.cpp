// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "ParticleLoader.hpp"
// #include "Pomdog/Experimental/Particle2D/detail/ParticleCurveKey.hpp"
// #include "Pomdog/Experimental/Particle2D/detail/ParticleCurveLerp.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleEmitterShapeBox.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleEmitterShapeSector.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleParameterConstant.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleParameterCurve.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleParameterRandom.hpp"
#include "Pomdog/Experimental/Particle2D/detail/ParticleParameterRandomCurves.hpp"
#include "Pomdog/Experimental/Particle2D/ParticleClip.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <rapidjson/document.h>
#include <utility>
#include <fstream>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace {

template <typename T>
T GetMemberAs(rapidjson::Value const& object, std::string const& name)
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(object.HasMember(name.c_str()));

    auto const& value = object[name.c_str()];
    POMDOG_ASSERT(value.IsNumber());

    if (value.IsInt()) {
        static_assert(std::is_convertible<decltype(value.GetInt()), T>::value, "");
        return value.GetInt();
    }

    POMDOG_ASSERT(value.IsDouble());
    static_assert(std::is_convertible<decltype(value.GetDouble()), T>::value, "");
    return value.GetDouble();
}

ParticleClip ReadParticleClip(rapidjson::Value const& object)
{
    ParticleClip clip;
    auto & emitter = clip.Emitter;

    emitter.StartDelay = 0;
    emitter.StartLifetime = GetMemberAs<float>(object, "particleLifespan");
    emitter.GravityModifier = 0;
    emitter.MaxParticles = GetMemberAs<float>(object, "maxParticles");

    POMDOG_ASSERT(emitter.MaxParticles > 0);
    POMDOG_ASSERT(emitter.StartLifetime > 0);
    emitter.EmissionRate = std::max(1.0f, emitter.MaxParticles / emitter.StartLifetime);

    ///@todo Not implemented
    //emitter.GravityModifier = object["gravity"].GetDouble();
    //emitter.? = object["particleLifespanVariance"].GetDouble();

    {
        auto duration = GetMemberAs<double>(object, "duration");

    #if DEBUG
        constexpr int DurationInfinity = -1;
        POMDOG_ASSERT(duration >= 0 || duration == DurationInfinity);
    #endif
        emitter.Looping = (duration <= 0);
        clip.Duration = Duration{std::max<double>(duration, emitter.StartLifetime)};
        POMDOG_ASSERT(clip.Duration > Duration::zero());
    }

    using Particles::ParticleCurveKey;
    using Particles::ParticleEmitterShapeBox;
    using Particles::ParticleParameterRandom;
    using Particles::ParticleParameterConstant;
    using Particles::ParticleParameterCurve;

    {
        auto speed = GetMemberAs<float>(object, "speed");
        auto speedVariance = GetMemberAs<float>(object, "speedVariance");

        POMDOG_ASSERT(speed >= 0);
        POMDOG_ASSERT(speedVariance >= 0);

        if (speedVariance > 0) {
            clip.StartSpeed = std::make_unique<ParticleParameterRandom<float>>(speed, speed + speedVariance);
        }
        else {
            clip.StartSpeed = std::make_unique<ParticleParameterConstant<float>>(speed);
        }
    }
//    {
//        auto startColor = object["startColor"].GetString();
//        auto startColorAlpha = object["startColorAlpha"].GetDouble();
//        auto startColorVariance = object["startColorVariance"].GetString();
//        auto startColorVarianceAlpha = object["startColorVarianceAlpha"].GetDouble();
//
//        auto finishColor = object["finishColor"].GetString();
//        auto finishColorAlpha = object["finishColorAlpha"].GetDouble();
//        auto finishColorVariance = object["finishColorVariance"].GetString();
//        auto finishColorVarianceAlpha = object["finishColorVarianceAlpha"].GetDouble();
//
//        clip.StartColor = std::make_unique<ParticleParameterRandom<float>>(speed, speed + speedVariance);
//        clip.ColorOverLifetime = std::make_unique<ParticleParameterConstant<float>>(speed);
//    }
    {
        ///@todo
        clip.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
        clip.ColorOverLifetime = std::make_unique<ParticleParameterCurve<Color>>(
            std::initializer_list<ParticleCurveKey<Color>>{
                {0.00f, Color{255, 255, 255, 0}},
                {0.02f, Color{255, 255, 255, 10}},
                {0.09f, Color{255, 250, 180, 100}},
                {0.15f, Color{255, 200, 180, 130}},
                {0.19f, Color{200, 130, 60, 255}},
                {0.24f, Color{190, 50, 10, 80}},
                {0.32f, Color{80, 24, 2, 20}},
                {1.00f, Color{0, 0, 0, 0}},
            });
    }
    {
        ///@todo Not implemented
        const float texturePixelWidth = 16.0f; // badcode
        const auto scale = 1.0f / texturePixelWidth;

        const auto startSize = GetMemberAs<float>(object, "startParticleSize");
        const auto startSizeVariance = GetMemberAs<float>(object, "startParticleSizeVariance");

        POMDOG_ASSERT(startSize >= 0);
        POMDOG_ASSERT(startSizeVariance >= 0);

        if (startSizeVariance > 0) {
            clip.StartSize = std::make_unique<ParticleParameterRandom<float>>(startSize * scale, (startSize + startSizeVariance) * scale);
        }
        else {
            clip.StartSize = std::make_unique<ParticleParameterConstant<float>>(startSize * scale);
        }

        const auto endSize = GetMemberAs<float>(object, "finishParticleSize");
        const auto ensSizeVariance = GetMemberAs<float>(object, "finishParticleSizeVariance");

        constexpr int StartSizeEqualToEndSize = -1;
        POMDOG_ASSERT(endSize >= 0 || endSize == StartSizeEqualToEndSize);
        POMDOG_ASSERT(ensSizeVariance >= 0);

        if (ensSizeVariance > 0) {
            clip.SizeOverLifetime = std::make_unique<ParticleParameterRandom<float>>(endSize * scale, (endSize + ensSizeVariance) * scale);
        }
        else if (endSize == StartSizeEqualToEndSize) {
            clip.SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(startSize * scale);
        }
        else {
            clip.SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(endSize * scale);
        }
    }
    {
        const Degree<float> rotation = GetMemberAs<float>(object, "rotationStart");
        const Degree<float> rotationVariance = GetMemberAs<float>(object, "rotationStartVariance");

        POMDOG_ASSERT(rotation >= 0);
        POMDOG_ASSERT(rotationVariance >= 0);

        if (rotationVariance > 0) {
            clip.StartRotation = std::make_unique<ParticleParameterRandom<Radian<float>>>(
                MathHelper::ToRadians(rotation), MathHelper::ToRadians(rotation + rotationVariance));
        }
        else {
            clip.StartRotation = std::make_unique<ParticleParameterConstant<Radian<float>>>(
                MathHelper::ToRadians(rotation));
        }
    }
    {
        auto rotation = GetMemberAs<float>(object, "rotationEnd");
        auto rotationVariance = GetMemberAs<float>(object, "rotationEndVariance");

        POMDOG_ASSERT(rotation >= 0);
        POMDOG_ASSERT(rotationVariance >= 0);

        if (rotationVariance > 0) {
            clip.RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<float>>>(
                rotation, rotation + rotationVariance);
        }
        else {
            clip.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<float>>>(rotation);
        }
    }
    {
        const auto halfWidth = object.HasMember("sourcePositionVariancex") ? GetMemberAs<float>(object, "sourcePositionVariancex"): 0.0f;
        const auto halfHeight = object.HasMember("sourcePositionVariancey") ? GetMemberAs<float>(object, "sourcePositionVariancey"): 0.0f;

        POMDOG_ASSERT(halfWidth >= 0);
        POMDOG_ASSERT(halfHeight >= 0);

        clip.Shape = std::make_unique<ParticleEmitterShapeBox>(halfWidth * 2, halfHeight * 2);
    }

    return clip;
}

} // unnamed namespace

ParticleClip ParticleLoader::LoadFromJson(AssetManager & assets, std::string const& assetName)
{
    POMDOG_ASSERT(!assetName.empty());

    using Detail::BinaryReader;

    auto binaryFile = assets.OpenStream(assetName);

    if (!binaryFile.Stream) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file");
    }

    if (binaryFile.SizeInBytes <= 0) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
    }

    auto json = BinaryReader::ReadString<char>(binaryFile.Stream, binaryFile.SizeInBytes);
    POMDOG_ASSERT(!json.empty());

    if (json.empty()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The file is too small");
    }

    rapidjson::Document doc;
    doc.Parse(json.data());

    if (doc.HasParseError() || !doc.IsObject() || doc.MemberBegin() == doc.MemberEnd()) {
        // FUS RO DAH
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to parse JSON");
    }

    auto member = doc.MemberBegin();

    if (!member->name.IsString() || !member->value.IsObject()) {
        // FUS RO DAH
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid file format");
    }

    POMDOG_ASSERT(member->name.IsString());
    POMDOG_ASSERT(member->value.IsObject());

    return ReadParticleClip(member->value);
}

ParticleClip ParticleLoader::Load(AssetManager & assets, std::string const& assetName)
{
    POMDOG_ASSERT(!assetName.empty());
    return LoadFromJson(assets, assetName);
}

} // namespace Detail
} // namespace Pomdog
