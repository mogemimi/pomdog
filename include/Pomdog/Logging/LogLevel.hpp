//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_LOGLEVEL_5E5C0F27_1A1E_45E8_881A_74F60B906155_HPP
#define POMDOG_LOGLEVEL_5E5C0F27_1A1E_45E8_881A_74F60B906155_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

///@~Japanese
/// @brief ログの出力における冗長レベルです。
enum class LogLevel: std::uint8_t {
	///@~Japanese
	/// @brief 回復することが不可能な致命的なエラーです。
	Critical = 0,

	///@~Japanese
	/// @brief 深刻なエラーや未定義の動作を引き起こしかねない場合に通知する警告です。
	Warning = 1,

	///@~Japanese
	/// @brief 日常的なログを示します。
	Info = 2,
	
	///@~Japanese
	/// @brief エンジンの内部処理に関するログを除くすべてのメッセージを示します。
	Verbose = 3,

	///@~Japanese
	/// @brief エンジンの内部処理に関わるログなどを含めたすべてのログを示します。
	Internal = 4,
};

}// namespace Pomdog

#endif // !defined(POMDOG_LOGLEVEL_5E5C0F27_1A1E_45E8_881A_74F60B906155_HPP)
