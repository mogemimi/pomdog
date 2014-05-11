//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONSTATE_35B967B3_9C05_4433_AFF4_9777DCED9525_HPP
#define POMDOG_ANIMATIONSTATE_35B967B3_9C05_4433_AFF4_9777DCED9525_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Application/DurationSeconds.hpp>

namespace Pomdog {

class AnimationClip;

//enum class AnimationWrapMode: std::uint8_t {
//	///@~Japanese
//	/// @brief アニメーションを 1 回だけ再生します。
//	/// @note アニメーションを再生した後、時間は 0 に戻り、アニメーションを終了します。
//	Once,
//	
//	///@~Japanese
//	/// @brief アニメーションを繰り返し再生します。
//	Loop,
//	
//	///@~Japanese
//	/// @brief アニメーションを再生した後、次に逆再生を行い、これを繰り返します。
//	PingPong
//};

//class AnimationBlendTree {
//
//};
//
//class AnimationBlendTreeNode {
//
//};
//
//class AnimationBlendLerp : public AnimationBlendTreeNode {
//public:
//	
//};

class AnimationState {
public:
	explicit AnimationState(std::shared_ptr<AnimationClip> const& animationClip);
	AnimationState(std::shared_ptr<AnimationClip> const& animationClip, float playbackRate, bool loop);

	std::shared_ptr<AnimationClip> const& Clip() const;

	DurationSeconds Time() const;
	void Time(DurationSeconds const& time);

	DurationSeconds Length() const;

	float PlaybackRate() const;
	void PlaybackRate(float playbackRate);
	
	bool Enabled() const;
	void Enabled(bool enabled);
	
	bool Loop() const;
	void Loop(bool Loop);
	
private:
	std::shared_ptr<AnimationClip> clip;
	DurationSeconds time;
	float playbackRate;
	//float weight;
	bool enabled;
	bool loop;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONSTATE_35B967B3_9C05_4433_AFF4_9777DCED9525_HPP)
