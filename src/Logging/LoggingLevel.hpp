//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LOGGINGLEVEL_HPP
#define POMDOG_LOGGINGLEVEL_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <pomdog/Config/FundamentalTypes.hpp>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Logging
/// @{

///@~English
/// @brief verbosity level of logging.
///@~Japanese
/// @brief ログの出力における冗長レベルです。トレースレベルとも言います。
enum class LoggingLevel: std::uint16_t
{
	///@~English 
	///@~Japanese 致命的なエラーです。回復することが不可能な状態を示します。
	Critical = 1,

	///@~English 
	///@~Japanese 特定の重要なイベントログのみ
	Brief = 2,

	///@~English 
	///@~Japanese 日常的なイベントログは全て
	Extensive = 3,

	///@~English 
	///@~Japanese 内部処理のエラーログを除く詳細なメッセージを示します
	Verbose = 4,

	///@~English 
	///@~Japanese デバッグログや内部処理エラーなど全て含めます
	Internal = 5
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_LOGGINGLEVEL_HPP)
