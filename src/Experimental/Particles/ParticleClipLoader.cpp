// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Particles/ParticleClipLoader.hpp"
#include "Pomdog/Application/FileSystem.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeBox.hpp"
#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeCone.hpp"
#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeHemisphere.hpp"
#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSector.hpp"
#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSphere.hpp"
#include "Pomdog/Experimental/Particles/Parameters/ParticleParameterConstant.hpp"
#include "Pomdog/Experimental/Particles/Parameters/ParticleParameterCurve.hpp"
#include "Pomdog/Experimental/Particles/Parameters/ParticleParameterRandom.hpp"
#include "Pomdog/Experimental/Particles/Parameters/ParticleParameterRandomCurves.hpp"
#include "Pomdog/Experimental/Particles/ParticleClip.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 5054)
#endif
#include <rapidjson/document.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include <fstream>
#include <utility>
#include <vector>

namespace Pomdog::Detail {
namespace {

using Particles::ParticleCurveKey;
using Particles::ParticleEmitterShapeBox;
using Particles::ParticleEmitterShapeCone;
using Particles::ParticleEmitterShapeHemisphere;
using Particles::ParticleEmitterShapeSector;
using Particles::ParticleEmitterShapeSphere;
using Particles::ParticleParameterConstant;
using Particles::ParticleParameterCurve;
using Particles::ParticleParameterRandom;

std::tuple<Vector3, std::unique_ptr<Error>>
ParseVector3(const rapidjson::Value& value)
{
    if (value.IsArray() &&
        (value.Size() == 3) &&
        (value[0].IsFloat() || value[0].IsInt()) &&
        (value[1].IsFloat() || value[1].IsInt()) &&
        (value[2].IsFloat() || value[2].IsInt())) {
        Vector3 vec;
        vec.X = value[0].GetFloat();
        vec.Y = value[1].GetFloat();
        vec.Z = value[2].GetFloat();
        return std::make_tuple(vec, nullptr);
    }
    return std::make_tuple(Vector3{}, Errors::New("member is not Vector3"));
}

[[maybe_unused]] std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, Vector3& vec)
{
    if (!value.HasMember(name)) {
        return Errors::New(StringHelper::Format("should have a '%s' member", name));
    }

    auto [res, err] = ParseVector3(value[name]);
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), StringHelper::Format("Value of '%s' should be Vector3", name));
    }
    vec = res;
    return nullptr;
}

std::tuple<Color, std::unique_ptr<Error>>
ParseColor(const rapidjson::Value& value)
{
    if (value.IsArray() &&
        (value.Size() == 4) &&
        (value[0].IsUint()) &&
        (value[1].IsUint()) &&
        (value[2].IsUint()) &&
        (value[3].IsUint())) {
        Color color;
        color.R = static_cast<std::uint8_t>(value[0].GetUint());
        color.G = static_cast<std::uint8_t>(value[1].GetUint());
        color.B = static_cast<std::uint8_t>(value[2].GetUint());
        color.A = static_cast<std::uint8_t>(value[3].GetUint());
        return std::make_tuple(color, nullptr);
    }
    return std::make_tuple(Color::TransparentBlack, Errors::New("member is not Color"));
}

std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, Color& vec)
{
    if (!value.HasMember(name)) {
        return Errors::New(StringHelper::Format("should have a '%s' member", name));
    }

    auto [res, err] = ParseColor(value[name]);
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), StringHelper::Format("Value of '%s' should be Color", name));
    }
    vec = res;
    return nullptr;
}

std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, int& result)
{
    if (!value.HasMember(name)) {
        return Errors::New(StringHelper::Format("should have a '%s' member", name));
    }
    auto& member = value[name];
    if (!member.IsInt()) {
        return Errors::New(StringHelper::Format("Value of '%s' should be int", name));
    }
    result = member.GetInt();
    return nullptr;
}

std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, double& result)
{
    if (!value.HasMember(name)) {
        return Errors::New(StringHelper::Format("should have a '%s' member", name));
    }
    auto& member = value[name];
    if (!member.IsDouble()) {
        return Errors::New(StringHelper::Format("Value of '%s' should be double", name));
    }
    result = member.GetDouble();
    return nullptr;
}

std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, float& result)
{
    if (!value.HasMember(name)) {
        return Errors::New(StringHelper::Format("should have a '%s' member", name));
    }
    auto& member = value[name];
    if (!member.IsFloat()) {
        return Errors::New(StringHelper::Format("Value of '%s' should be double", name));
    }
    result = member.GetFloat();
    return nullptr;
}

std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, bool& result)
{
    if (!value.HasMember(name)) {
        return Errors::New(StringHelper::Format("should have a '%s' member", name));
    }
    auto& member = value[name];
    if (!member.IsBool()) {
        return Errors::New(StringHelper::Format("Value of '%s' should be double", name));
    }
    result = member.GetBool();
    return nullptr;
}

std::unique_ptr<Error>
ParseMember(const rapidjson::Value& value, const char* name, Duration& result)
{
    double v = result.count();
    auto err = ParseMember(value, name, v);
    result = Duration{v};
    return err;
}

std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
ReadParticleClip(const rapidjson::Value& object)
{
    auto particleClip = std::make_shared<ParticleClip>();
    auto& clip = *particleClip;

    if (auto err = ParseMember(object, "duration", clip.Duration); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "emission_rate", clip.EmissionRate); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "emission_rate_over_time", clip.EmissionRateOverTime); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "start_lifetime", clip.StartLifetime); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "start_delay", clip.StartDelay); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "gravity_modifier", clip.GravityModifier); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "max_particles", clip.MaxParticles); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = ParseMember(object, "looping", clip.Looping); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }

    if ((clip.MaxParticles <= 0) || (clip.MaxParticles > 4096)) {
        auto err = Errors::New("invalid range, MaxParticles");
        return std::make_tuple(nullptr, std::move(err));
    }
    if (clip.StartLifetime <= 0) {
        auto err = Errors::New("StartLifetime should be > 0");
        return std::make_tuple(nullptr, std::move(err));
    }
    if (clip.EmissionRate <= 0) {
        auto err = Errors::New("EmissionRate should be > 0");
        return std::make_tuple(nullptr, std::move(err));
    }
    if (clip.EmissionRateOverTime <= 0) {
        auto err = Errors::New("EmissionRateOverTime should be > 0");
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto readShape = [&]() -> std::unique_ptr<Error> {
        if (!object.HasMember("shape")) {
            return Errors::New("should have a 'shape' member");
        }
        auto& shape = object["shape"];
        auto& shapeType = shape["type"];

        if (shapeType.GetString() == std::string_view{"box"}) {
            const auto width = shape["width"].GetFloat();
            const auto height = shape["height"].GetFloat();

            auto depth = 0.0f;
            if (shape.HasMember("depth")) {
                depth = shape["depth"].GetFloat();
            }

            POMDOG_ASSERT(width >= 0);
            POMDOG_ASSERT(height >= 0);
            clip.Shape = std::make_unique<ParticleEmitterShapeBox>(Vector3{width, height, depth});
        }
        else if (shapeType.GetString() == std::string_view{"cone"}) {
            const auto radius = shape["radius"].GetFloat();
            const auto height = shape["height"].GetFloat();
            clip.Shape = std::make_unique<ParticleEmitterShapeCone>(radius, height);
        }
        else if (shapeType.GetString() == std::string_view{"hemisphere"}) {
            const auto radius = shape["radius"].GetFloat();
            clip.Shape = std::make_unique<ParticleEmitterShapeHemisphere>(radius);
        }
        else if (shapeType.GetString() == std::string_view{"sector"}) {
            const auto centralAngle = shape["angle"].GetFloat();
            auto radian = Math::ToRadians(centralAngle);
            clip.Shape = std::make_unique<ParticleEmitterShapeSector>(std::move(radian));
        }
        else if (shapeType.GetString() == std::string_view{"sphere"}) {
            const auto radius = shape["radius"].GetFloat();
            clip.Shape = std::make_unique<ParticleEmitterShapeSphere>(radius);
        }
        return nullptr;
    };
    if (auto err = readShape(); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }

    const auto readFloatParameter = [&](const char* name, std::unique_ptr<Detail::Particles::ParticleParameter<float>>& result) -> std::unique_ptr<Error> {
        if (!object.HasMember(name)) {
            return Errors::New("should have a 'start_speed' member");
        }
        auto& shape = object[name];

        if (shape["type"].GetString() == std::string_view{"constant"}) {
            const auto value = shape["value"].GetFloat();
            result = std::make_unique<ParticleParameterConstant<float>>(value);
        }
        else if (shape["type"].GetString() == std::string_view{"random"}) {
            const auto min = shape["min"].GetFloat();
            const auto max = shape["max"].GetFloat();
            result = std::make_unique<ParticleParameterRandom<float>>(min, max);
        }
        else if (shape["type"].GetString() == std::string_view{"curve"}) {
            const auto& pointsArray = shape["points"].GetArray();

            std::vector<ParticleCurveKey<float>> points;
            points.reserve(pointsArray.Size());
            for (const auto& p : pointsArray) {
                ParticleCurveKey<float> point;
                point.TimeSeconds = p["t"].GetFloat();
                point.Value = p["v"].GetFloat();
                points.push_back(std::move(point));
            }

            result = std::make_unique<ParticleParameterCurve<float>>(std::move(points));
        }
        return nullptr;
    };
    const auto readRadianParameter = [&](const char* name, std::unique_ptr<Detail::Particles::ParticleParameter<Radian<float>>>& result) -> std::unique_ptr<Error> {
        if (!object.HasMember(name)) {
            return Errors::New("should have a 'start_speed' member");
        }
        auto& shape = object[name];

        if (shape["type"].GetString() == std::string_view{"constant"}) {
            const auto value = Math::ToRadians(shape["value"].GetFloat());
            result = std::make_unique<ParticleParameterConstant<Radian<float>>>(value);
        }
        else if (shape["type"].GetString() == std::string_view{"random"}) {
            const auto min = Math::ToRadians(shape["min"].GetFloat());
            const auto max = Math::ToRadians(shape["max"].GetFloat());
            result = std::make_unique<ParticleParameterRandom<Radian<float>>>(min, max);
        }
        else if (shape["type"].GetString() == std::string_view{"curve"}) {
            const auto& pointsArray = shape["points"].GetArray();

            std::vector<ParticleCurveKey<Radian<float>>> points;
            points.reserve(pointsArray.Size());
            for (const auto& p : pointsArray) {
                ParticleCurveKey<Radian<float>> point;
                point.TimeSeconds = p["t"].GetFloat();
                point.Value = Math::ToRadians((p["v"].GetFloat()));
                points.push_back(std::move(point));
            }

            result = std::make_unique<ParticleParameterCurve<Radian<float>>>(std::move(points));
        }
        return nullptr;
    };
    const auto readColorParameter = [&](const char* name, std::unique_ptr<Detail::Particles::ParticleParameter<Color>>& result) -> std::unique_ptr<Error> {
        if (!object.HasMember(name)) {
            return Errors::New("should have a 'start_speed' member");
        }
        auto& shape = object[name];

        if (shape["type"].GetString() == std::string_view{"constant"}) {
            auto [value, err] = ParseColor(shape["value"]);
            if (err != nullptr) {
                return std::move(err);
            }
            result = std::make_unique<ParticleParameterConstant<Color>>(value);
        }
        else if (shape["type"].GetString() == std::string_view{"random"}) {
            auto [min, minErr] = ParseColor(shape["min"]);
            if (minErr != nullptr) {
                return std::move(minErr);
            }
            auto [max, maxErr] = ParseColor(shape["max"]);
            if (maxErr != nullptr) {
                return std::move(maxErr);
            }
            result = std::make_unique<ParticleParameterRandom<Color>>(min, max);
        }
        else if (shape["type"].GetString() == std::string_view{"curve"}) {
            const auto& pointsArray = shape["points"].GetArray();

            std::vector<ParticleCurveKey<Color>> points;
            points.reserve(pointsArray.Size());
            for (const auto& p : pointsArray) {
                ParticleCurveKey<Color> point;
                point.TimeSeconds = p["t"].GetFloat();
                if (auto err = ParseMember(p, "v", point.Value); err != nullptr) {
                    return err;
                }
                points.push_back(std::move(point));
            }

            result = std::make_unique<ParticleParameterCurve<Color>>(std::move(points));
        }
        return nullptr;
    };
    if (auto err = readFloatParameter("start_speed", clip.StartSpeed); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = readRadianParameter("start_rotation", clip.StartRotation); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = readRadianParameter("rotation_over_lifetime", clip.RotationOverLifetime); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = readFloatParameter("start_size", clip.StartSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = readFloatParameter("size_over_lifetime", clip.SizeOverLifetime); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = readColorParameter("start_color", clip.StartColor); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    if (auto err = readColorParameter("color_over_lifetime", clip.ColorOverLifetime); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(particleClip), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<ParticleClip>, std::unique_ptr<Error>>
AssetLoader<ParticleClip>::operator()(
    [[maybe_unused]] AssetManager& assets, const std::string& filePath)
{
    using Detail::BinaryReader;

    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = Errors::New("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = Errors::Wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    POMDOG_ASSERT(stream);

    if (byteLength <= 0) {
        auto err = Errors::New("the file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto json = BinaryReader::ReadArray<char>(stream, byteLength);
    POMDOG_ASSERT(!json.empty());

    if (json.empty()) {
        auto err = Errors::New("the file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }
    json.push_back('\0');

    rapidjson::Document doc;
    doc.Parse(json.data());

    if (doc.HasParseError() || !doc.IsObject() || doc.MemberBegin() == doc.MemberEnd()) {
        auto err = Errors::New("failed to parse JSON " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    return ReadParticleClip(doc);
}

} // namespace Pomdog::Detail
