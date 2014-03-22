//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LOGENTRY_F0B169C2_B3E2_4144_939B_F1FBE89BF3AC_HPP
#define POMDOG_LOGENTRY_F0B169C2_B3E2_4144_939B_F1FBE89BF3AC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include "LogLevel.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Logging
/// @{

///@~Japanese
/// @brief メッセージ情報を含むログエントリです。
class LogEntry {
public:
	///@~Japanese
	/// @brief ログメッセージまたはトレースメッセージです。
	std::string Message;
	
	///@~Japanese
	/// @brief ソースとなるチャンネルです。
	std::string Source;

	///@~Japanese
	/// @brief ログの冗長レベルです。
	LogLevel Verbosity;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_LOGENTRY_F0B169C2_B3E2_4144_939B_F1FBE89BF3AC_HPP)
