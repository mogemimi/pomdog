//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLECLIP_CC5B71B4_5A11_4D2A_8225_5633A4BCD516_HPP
#define POMDOG_PARTICLECLIP_CC5B71B4_5A11_4D2A_8225_5633A4BCD516_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "detail/ParticleEmitterShape.hpp"
#include "detail/ParticleParameter.hpp"
#include "ParticleEmitter.hpp"
#include "Pomdog/Application/DurationSeconds.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class ParticleClip {
public:
	ParticleEmitter Emitter;

	///@note StartSpeed (pixel/second)
	std::unique_ptr<Details::Particles::ParticleParameter<float>> StartSpeed;
	
	std::unique_ptr<Details::Particles::ParticleEmitterShape> Shape;
	
	///@brief パーティクルの初期色を指定します。
	std::unique_ptr<Details::Particles::ParticleParameter<Color>> StartColor;
	
	///@brief パーティクルの生存期間に応じた色を指定します。
	///@note このパラメータで指定された色と各パーティクルの初期色を乗算した結果が最終的な色となります。
	std::unique_ptr<Details::Particles::ParticleParameter<Color>> ColorOverLifetime;
	
	///@brief パーティクルの初期回転角を指定します。
	std::unique_ptr<Details::Particles::ParticleParameter<Radian<float>>> StartRotation;
	
	///@brief パーティクルの生存期間に応じた 1 秒あたりの回転速度を指定します。
	std::unique_ptr<Details::Particles::ParticleParameter<Radian<float>>> RotationOverLifetime;
	
	///@brief パーティクルの初期サイズを指定します。
	std::unique_ptr<Details::Particles::ParticleParameter<float>> StartSize;
	
	///@brief パーティクルの生存期間に応じたサイズを指定します。
	std::unique_ptr<Details::Particles::ParticleParameter<float>> SizeOverLifetime;
	
	DurationSeconds Duration = std::chrono::seconds(5);
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLECLIP_CC5B71B4_5A11_4D2A_8225_5633A4BCD516_HPP)
