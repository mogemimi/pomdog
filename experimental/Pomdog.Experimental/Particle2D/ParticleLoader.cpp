// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "ParticleLoader.hpp"
//#include "detail/ParticleCurveKey.hpp"
//#include "detail/ParticleCurveLerp.hpp"
#include "detail/ParticleEmitterShapeBox.hpp"
#include "detail/ParticleEmitterShapeSector.hpp"
#include "detail/ParticleParameterConstant.hpp"
#include "detail/ParticleParameterCurve.hpp"
#include "detail/ParticleParameterRandom.hpp"
#include "detail/ParticleParameterRandomCurves.hpp"
#include "ParticleClip.hpp"
#include <rapidjson/document.h>
#include <utility>
#include <fstream>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace {
//-----------------------------------------------------------------------
static std::vector<char> ReadBinaryFile(std::string const& path)
{
	std::ifstream stream(path, std::ios::in | std::ios::binary);

	stream.seekg(0, stream.end);
	auto const length = static_cast<std::size_t>(stream.tellg());
	stream.seekg(0, stream.beg);

	std::vector<char> result(length + 1, '\0');
	stream.read(result.data(), length);
	return std::move(result);
}
//-----------------------------------------------------------------------
//static ParticleClip CreateEmitterFireBlock()
//{
//	using Particles::ParticleCurveKey;
//	using Particles::ParticleEmitterShapeBox;
//	using Particles::ParticleParameterRandom;
//	using Particles::ParticleParameterConstant;
//	using Particles::ParticleParameterCurve;
//
//	ParticleClip clip;
//
//	clip.Emitter.MaxParticles = 1024;
//	clip.Emitter.EmissionRate = 128*2;
//	//clip.Emitter.Looping = false;
//	clip.Emitter.StartLifetime = 1.8f;
//	//clip.Emitter.GravityModifier = 100.0f;
//
//	clip.Duration = std::chrono::milliseconds(10);
//
//	//clip.Shape = std::make_unique<ParticleEmitterShapeSector>(MathConstants<float>::PiOver4());
//	clip.Shape = std::make_unique<ParticleEmitterShapeBox>(0, 100);
//
//	clip.StartSpeed = std::make_unique<ParticleParameterRandom<float>>(40.0f, 128.0f);
//	//clip.StartSpeed = std::make_unique<ParticleParameterConstant<float>>(-128.0f);
//
////	clip.StartSpeed = std::make_unique<ParticleParameterCurve<float>>(
////		std::initializer_list<ParticleCurveKey<float>>{
////			{0.00f, 0.0f},
////			{0.10f, -0.5f},
////			{0.15f, -1.0f},
////			{0.20f, -0.5f},
////			{0.40f, 0.0f},
////			{0.50f, 0.5f},
////			{0.70f, 1.0f},
////			{0.80f, 0.5f},
////			{1.00f, 0.0f},
////		});
//
//	clip.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
//	//emitter.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
//	//emitter.StartColor = std::make_unique<ParticleParameterRandom<Color>>(Color::Black, Color::White);
//
//	//clip.ColorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
//	//clip.ColorOverLifetime = std::make_unique<ParticleParameterRandom<Color>>(Color::Yellow, Color::Black);
//	clip.ColorOverLifetime = std::make_unique<ParticleParameterCurve<Color>>(
//		std::initializer_list<ParticleCurveKey<Color>>{
//			{0.00f, Color{255, 255, 255, 0}},
//			{0.02f, Color{255, 255, 255, 10}},
//			{0.09f, Color{255, 250, 180, 100}},
//			{0.15f, Color{255, 200, 180, 130}},
//			{0.19f, Color{200, 130, 60, 255}},
//			{0.24f, Color{190, 50, 10, 80}},
//			{0.32f, Color{80, 24, 2, 20}},
//			{1.00f, Color{0, 0, 0, 0}},
//		});
//
//	//clip.StartRotation = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
//	clip.StartRotation = std::make_unique<ParticleParameterRandom<Radian<float>>>(
//		0, MathConstants<float>::TwoPi());
//
//	//clip.Emitter.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
//	clip.RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<float>>>(
//		-MathConstants<float>::PiOver4(), MathConstants<float>::PiOver4());
//
//	//clip.StartSize = std::make_unique<ParticleParameterConstant<float>>(1.0f);
//	clip.StartSize = std::make_unique<ParticleParameterRandom<float>>(0.8f, 1.2f);
//
//	//clip.SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(1.0f);
//	clip.SizeOverLifetime = std::make_unique<ParticleParameterCurve<float>>(
//		std::initializer_list<ParticleCurveKey<float>>{
//			{0.00f, 0.0f},
//			{0.03f, 0.5f},
//			{0.10f, 0.8f},
//			{0.15f, 1.0f},
//			{0.60f, 0.8f},
//			{1.00f, 0.0f},
//		});
//
//	return std::move(clip);
//}
//-----------------------------------------------------------------------
template <typename T>
static T GetMemberAs(rapidjson::Value const& object, char const* name)
{
	POMDOG_ASSERT(object.HasMember(name));

	auto const& value = object[name];
	POMDOG_ASSERT(value.IsNumber());

	if (value.IsInt()) {
		static_assert(std::is_convertible<decltype(value.GetInt()), T>::value, "");
		return value.GetInt();
	}

	POMDOG_ASSERT(value.IsDouble());
	static_assert(std::is_convertible<decltype(value.GetDouble()), T>::value, "");
	return value.GetDouble();
}
//-----------------------------------------------------------------------
static ParticleClip ReadParticleClip(rapidjson::Value const& object)
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
//	{
//		auto startColor = object["startColor"].GetString();
//		auto startColorAlpha = object["startColorAlpha"].GetDouble();
//		auto startColorVariance = object["startColorVariance"].GetString();
//		auto startColorVarianceAlpha = object["startColorVarianceAlpha"].GetDouble();
//
//		auto finishColor = object["finishColor"].GetString();
//		auto finishColorAlpha = object["finishColorAlpha"].GetDouble();
//		auto finishColorVariance = object["finishColorVariance"].GetString();
//		auto finishColorVarianceAlpha = object["finishColorVarianceAlpha"].GetDouble();
//
//		clip.StartColor = std::make_unique<ParticleParameterRandom<float>>(speed, speed + speedVariance);
//		clip.ColorOverLifetime = std::make_unique<ParticleParameterConstant<float>>(speed);
//	}
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

	return std::move(clip);
}

}// unnamed namespace

ParticleClip ParticleLoader::LoadFromJson(AssetManager & assets, std::string const& assetName)
{
	POMDOG_ASSERT(!assetName.empty());

	auto path = assets.RootDirectory() + "/" + assetName;
	auto json = ReadBinaryFile(path);

	POMDOG_ASSERT(!json.empty());

	if (json.empty()) {
		///@todo Not implemented
		// Error
	}

	rapidjson::Document doc;
	doc.Parse(json.data());

	if (doc.HasParseError() || !doc.IsObject() || doc.MemberBegin() == doc.MemberEnd())
	{
		///@todo Not implemented
		// Error
		POMDOG_ASSERT(false);
	}

	auto member = doc.MemberBegin();

	if (!member->name.IsString() || !member->value.IsObject())
	{
		///@todo Not implemented
		// Error
		POMDOG_ASSERT(false);
	}

	POMDOG_ASSERT(member->name.IsString());
	POMDOG_ASSERT(member->value.IsObject());

	return ReadParticleClip(member->value);
}
//-----------------------------------------------------------------------
ParticleClip ParticleLoader::Load(AssetManager & assets, std::string const& assetName)
{
	POMDOG_ASSERT(!assetName.empty());
	return LoadFromJson(assets, assetName);
}

}// namespace Detail
}// namespace Pomdog
