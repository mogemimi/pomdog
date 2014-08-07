//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP
#define POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "ParticleParameter.hpp"
#include "ParticleEmitterShape.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>

namespace Pomdog {

class ParticleEmitter {
public:
	DurationSeconds Duration {5.0};
	bool Looping = true;
	float StartDelay = 0; // bool Prewarm;
	
	///@note StartSpeed (pixel/second)
	std::unique_ptr<ParticleParameter<float>> StartSpeed;
	
	std::unique_ptr<ParticleEmitterShape> Shape;
	
	///@brief パーティクルの初期色を指定します。
	std::unique_ptr<ParticleParameter<Color>> StartColor;
	
	///@brief パーティクルの生存期間に応じた色を指定します。
	///@note このパラメータで指定された色と各パーティクルの初期色を乗算した結果が最終的な色となります。
	std::unique_ptr<ParticleParameter<Color>> ColorOverLifetime;
	
	///@brief パーティクルの初期回転角を指定します。
	std::unique_ptr<ParticleParameter<Radian<float>>> StartRotation;
	
	///@brief パーティクルの生存期間に応じた 1 秒あたりの回転速度を指定します。
	std::unique_ptr<ParticleParameter<Radian<float>>> RotationOverLifetime;
	
	///@brief パーティクルの初期サイズを指定します。
	std::unique_ptr<ParticleParameter<float>> StartSize;
	
	///@brief パーティクルの生存期間に応じたサイズを指定します。
	std::unique_ptr<ParticleParameter<float>> SizeOverLifetime;
	
	float StartLifetime = 1;

	float GravityModifier = 0;
	
	///@brief 発生させるパーティクルの最大数
	std::uint16_t MaxParticles = 128;
	
	///@brief 1 秒間に発生させるパーティクルの数
	std::uint16_t EmissionRate = 16;
	
	//float InheritVelocity;
	//ParticleSimulationSpace SimulationSpace;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP)
