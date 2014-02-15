//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LOGCHANNEL_4C388D05_4E15_4A29_8E76_D0A535C514F5_HPP
#define POMDOG_LOGCHANNEL_4C388D05_4E15_4A29_8E76_D0A535C514F5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <functional>
#include <string>
#include "../Config/Export.hpp"
#include "../Event/EventHandler.hpp"
#include "LogEntry.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Logging
/// @{

///@~Japanese
/// @brief 登録されたリスナーに向けてログを送り出すロガーです。
class POMDOG_EXPORT LogChannel
{
public:
	explicit LogChannel(std::string const& name);

	LogChannel() = delete;
	LogChannel(LogChannel const&) = delete;
	LogChannel & operator=(LogChannel const&) = delete;
	LogChannel(LogChannel &&) = delete;
	LogChannel & operator=(LogChannel &&) = delete;
	
	~LogChannel() = default;

	///@~Japanese
	/// @brief メッセージを送ります。
	void LogMessage(std::string const& message, LoggingLevel verbosity);

	///@~Japanese
	/// @brief 指定されたスロットを接続します。
	EventConnection Connect(std::function<void(LogEntry const&)> const& slot);

	///@~Japanese
	/// @brief チャンネルを識別する名前を取得します。
	std::string const& GetName() const;

	///@~Japanese
	/// @brief ロギングの冗長レベルを取得します。
	LoggingLevel GetLevel() const;

	///@~Japanese
	/// @brief ロギングの冗長レベルを設定します。
	void SetLevel(LoggingLevel threshold);

private:
	EventHandler eventHandler;
	std::string name;
	LoggingLevel threshold;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_LOGCHANNEL_4C388D05_4E15_4A29_8E76_D0A535C514F5_HPP)
