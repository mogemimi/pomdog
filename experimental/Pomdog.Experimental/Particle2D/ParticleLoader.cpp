//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

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

namespace Pomdog {
namespace Details {
namespace {
//-----------------------------------------------------------------------
static ParticleClip CreateEmitterFireBlock()
{
	ParticleClip clip;

	clip.Emitter.MaxParticles = 1024;
	clip.Emitter.EmissionRate = 128*2;
	//clip.Emitter.Looping = false;
	clip.Emitter.StartLifetime = 1.8f;
	//clip.Emitter.GravityModifier = 100.0f;
	
	clip.Duration = std::chrono::milliseconds(10);
	
	//clip.Shape = std::make_unique<ParticleEmitterShapeSector>(MathConstants<float>::PiOver4());
	clip.Shape = std::make_unique<ParticleEmitterShapeBox>(0, 100);
	
	clip.StartSpeed = std::make_unique<ParticleParameterRandom<float>>(40.0f, 128.0f);
	//clip.StartSpeed = std::make_unique<ParticleParameterConstant<float>>(-128.0f);
	
//	clip.StartSpeed = std::make_unique<ParticleParameterCurve<float>>(
//		std::initializer_list<ParticleCurveKey<float>>{
//			{0.00f, 0.0f},
//			{0.10f, -0.5f},
//			{0.15f, -1.0f},
//			{0.20f, -0.5f},
//			{0.40f, 0.0f},
//			{0.50f, 0.5f},
//			{0.70f, 1.0f},
//			{0.80f, 0.5f},
//			{1.00f, 0.0f},
//		});
	
	clip.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//emitter.StartColor = std::make_unique<ParticleParameterRandom<Color>>(Color::Black, Color::White);
	
	//clip.ColorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::White);
	//clip.ColorOverLifetime = std::make_unique<ParticleParameterRandom<Color>>(Color::Yellow, Color::Black);
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
	
	//clip.StartRotation = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
	clip.StartRotation = std::make_unique<ParticleParameterRandom<Radian<float>>>(
		0, MathConstants<float>::TwoPi());
	
	//clip.Emitter.RotationOverLifetime = std::make_unique<ParticleParameterConstant<Radian<float>>>(0);
	clip.RotationOverLifetime = std::make_unique<ParticleParameterRandom<Radian<float>>>(
		-MathConstants<float>::PiOver4(), MathConstants<float>::PiOver4());
	
	//clip.StartSize = std::make_unique<ParticleParameterConstant<float>>(1.0f);
	clip.StartSize = std::make_unique<ParticleParameterRandom<float>>(0.8f, 1.2f);
	
	//clip.SizeOverLifetime = std::make_unique<ParticleParameterConstant<float>>(1.0f);
	clip.SizeOverLifetime = std::make_unique<ParticleParameterCurve<float>>(
		std::initializer_list<ParticleCurveKey<float>>{
			{0.00f, 0.0f},
			{0.03f, 0.5f},
			{0.10f, 0.8f},
			{0.15f, 1.0f},
			{0.60f, 0.8f},
			{1.00f, 0.0f},
		});

	return std::move(clip);
}

}// unnamed namespace

ParticleClip ParticleLoader::Load()
{
	return CreateEmitterFireBlock();
}

}// namespace Details
}// namespace Pomdog
