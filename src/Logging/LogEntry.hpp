//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LOGENTRY_HPP
#define POMDOG_LOGENTRY_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "LoggingLevel.hpp"
#include <string>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Logging
/// @{

///@~English
///@~Japanese
/// @brief メッセージ情報を含むログエントリです。
class LogEntry
{
public:
	///@~English 
	///@~Japanese
	/// @brief ログメッセージあるいはトレースメッセージです。
	std::string Message;

	///@~English 
	///@~Japanese
	/// @brief ログの冗長レベルです。
	LoggingLevel Verbosity;

public:
	///@~Japanese
	/// @brief コンストラクタです。
	/// @param message ログメッセージを指定します。
	/// @param verbosity ログ出力レベル（冗長レベル）を指定します。
	LogEntry(std::string const& message, LoggingLevel verbosity)
		: Message    (message)
		, Verbosity  (verbosity)
	{}
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_LOGENTRY_HPP)
