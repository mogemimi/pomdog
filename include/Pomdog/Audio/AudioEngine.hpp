// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOENGINE_0BAB9209_2564_48EB_A65A_F3B826A56E02_HPP
#define POMDOG_AUDIOENGINE_0BAB9209_2564_48EB_A65A_F3B826A56E02_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT AudioEngine {
public:
	AudioEngine();
	~AudioEngine();

	AudioEngine(AudioEngine const&) = delete;
	AudioEngine & operator=(AudioEngine const&) = delete;
	AudioEngine(AudioEngine &&) = default;
	AudioEngine & operator=(AudioEngine &&) = default;

	///@~Japanese
	/// @brief すべてのサウンドに影響を与えるマスターボリュームを取得します。
	/// @return 0.0f（無音） ~ 1.0f（音量の最大値） の範囲で現在の音量を返却します。
	float MasterVolume() const;

	///@~Japanese
	/// @brief すべてのサウンドに影響を与えるマスターボリュームを設定します。
	/// @param volume 0.0f（無音） ~ 1.0f（音量の最大値） の範囲でボリュームを指定してください。
	void MasterVolume(float volume);

	Detail::SoundSystem::NativeAudioEngine* NativeAudioEngine();

private:
	std::unique_ptr<Detail::SoundSystem::NativeAudioEngine> nativeAudioEngine;
};

}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOENGINE_0BAB9209_2564_48EB_A65A_F3B826A56E02_HPP)
