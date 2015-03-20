// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SOUNDEFFECT_0793F2D9_5D45_44A0_BA2A_66B75D99D015_HPP
#define POMDOG_SOUNDEFFECT_0793F2D9_5D45_44A0_BA2A_66B75D99D015_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "SoundState.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstdint>

namespace Pomdog {

class AudioClip;
class AudioEmitter;
class AudioListener;
class AudioEngine;
enum class SoundState: std::uint8_t;

class POMDOG_EXPORT SoundEffect {
public:
	SoundEffect(AudioEngine & audioEngine,
		std::shared_ptr<AudioClip> const& audioClip, bool isLooped);

	~SoundEffect();

	SoundEffect(SoundEffect const&) = delete;
	SoundEffect(SoundEffect &&) = default;
	SoundEffect & operator=(SoundEffect const&) = delete;
	SoundEffect & operator=(SoundEffect &&) = default;

	///@~Japanese
	/// @brief サウンドを一時停止します。
	void Pause();

	///@~Japanese
	/// @brief サウンドを再生（あるいは一時停止状態から再開）します。
	void Play();

	/// サウンドを停止します。
	void Stop();

	void Apply3D(AudioListener const& listener, AudioEmitter const& emitter);

	bool IsLooped() const;

	void ExitLoop();

	///@~Japanese
	/// @brief 現在のサウンドの状態を取得します。
	SoundState State() const;

	///@~Japanese
	/// @brief サウンドの定位を取得します。
	/// @return -1（左） ~ 1（右）の範囲で返却します。0 が中央です。
	float Pan() const;

	///@~Japanese
	/// @brief サウンドの定位を設定します。
	/// @param panpot -1（左） ~ 1（右） の範囲で指定します。 0 が中央です。
	void Pan(float panpot);

	///@~Japanese
	/// @brief サウンドのピッチ（音の高さ）を取得します。
	/// @return -1（1オクターブ下） ~ 1（1オクターブ上） の範囲で返却します。0 がピッチの標準値です。
	float Pitch() const;

	///@~Japanese
	/// @brief サウンドのピッチ（音の高さ）を設定します。
	/// @param pitch -1（1オクターブ下） ~ 1（1オクターブ上） の範囲で返却します。0 がピッチの標準値です。
	void Pitch(float pitch);

	///@~Japanese
	/// @brief サウンドの音量を取得します。
	/// @return 0（無音） ~ 1（最大音量）の範囲で返却します。
	float Volume() const;

	///@~Japanese
	/// @brief サウンドの音量を設定します。
	/// @param volume 0（無音） ~ 1（最大音量）の範囲で指定します。
	void Volume(float volume);

private:
	std::unique_ptr<Detail::SoundSystem::NativeSoundEffect> nativeSoundEffect;
	float pitch;
	float volume;
	SoundState state;
	bool isLooped;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SOUNDEFFECT_0793F2D9_5D45_44A0_BA2A_66B75D99D015_HPP)
