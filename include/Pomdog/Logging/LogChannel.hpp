//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_LOGCHANNEL_4C388D05_4E15_4A29_8E76_D0A535C514F5_HPP
#define POMDOG_LOGCHANNEL_4C388D05_4E15_4A29_8E76_D0A535C514F5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Event/Signal.hpp"
#include <functional>
#include <string>

namespace Pomdog {

class LogEntry;

///@~Japanese
/// @brief 登録されたリスナーに向けてログを送り出すロガーです。
class POMDOG_EXPORT LogChannel {
public:
	explicit LogChannel(std::string const& name);

	LogChannel() = default;
	LogChannel(LogChannel const&) = delete;
	LogChannel(LogChannel &&) = default;
	LogChannel & operator=(LogChannel const&) = delete;
	LogChannel & operator=(LogChannel &&) = default;

	///@~Japanese
	/// @brief メッセージを送ります。
	void Log(std::string const& message, LogLevel verbosity);

	///@~Japanese
	/// @brief メッセージを送ります。
	void Log(LogEntry const& logEntry);
	
	///@~Japanese
	/// @brief 指定されたスロットを接続します。
	EventConnection Connect(std::function<void(LogEntry const&)> const& slot);

	///@~Japanese
	/// @brief 指定されたスロットを接続します。
	EventConnection Connect(std::function<void(LogEntry const&)> && slot);

	///@~Japanese
	/// @brief チャンネルを識別する名前を取得します。
	std::string const& Name() const;

	///@~Japanese
	/// @brief ロギングの冗長レベルを取得します。
	LogLevel Level() const;

	///@~Japanese
	/// @brief ロギングの冗長レベルを設定します。
	void Level(LogLevel level);

	///@~Japanese
	/// @brief 接続数を取得します。
	std::size_t ConnectionCount() const;

private:
	Signal<void(LogEntry const&)> signal;
	std::string name;
	LogLevel level;
};

}// namespace Pomdog

#endif // !defined(POMDOG_LOGCHANNEL_4C388D05_4E15_4A29_8E76_D0A535C514F5_HPP)
