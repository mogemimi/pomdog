//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_LOG_B5157F87_E1D1_460C_8346_161983EF05D9_HPP
#define POMDOG_LOG_B5157F87_E1D1_460C_8346_161983EF05D9_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <string>
#include <functional>
#include "../Config/Export.hpp"
#include "LogLevel.hpp"

namespace Pomdog {

class EventConnection;
class LogEntry;
class LogStream;

class POMDOG_EXPORT Log {
public:
	///@~Japanese
	/// @brief デフォルトのチャンネルに接続します。
	static EventConnection Connect(std::function<void(LogEntry const&)> const& slot);
	
	///@~Japanese
	/// @brief デフォルトのチャンネルに接続します。
	static EventConnection Connect(std::function<void(LogEntry const&)> && slot);
	
	///@~Japanese
	/// @brief 指定されたチャンネルに接続します。
	/// @remarks 指定されたチャンネルが存在しない場合、新しくチャンネルを作成します。
	static EventConnection Connect(std::string const& channelName, std::function<void(LogEntry const&)> const& slot);

	///@~Japanese
	/// @brief 指定されたチャンネルに接続します。
	/// @remarks 指定されたチャンネルが存在しない場合、新しくチャンネルを作成します。
	static EventConnection Connect(std::string const& channelName, std::function<void(LogEntry const&)> && slot);

	///@~Japanese
	/// @brief デフォルトのチャンネルの冗長レベルを取得します。
	static LogLevel GetLevel();

	///@~Japanese
	/// @brief デフォルトのチャンネルの冗長レベルを設定します。
	static void SetLevel(LogLevel verbosity);

	///@~Japanese
	/// @brief 指定されたチャンネルの冗長レベルを取得します。
	static LogLevel GetLevel(std::string const& channelName);

	///@~Japanese
	/// @brief 指定されたチャンネルの冗長レベルを設定します。
	static void SetLevel(std::string const& channelName, LogLevel verbosity);

	///@~Japanese
	/// @brief デフォルトのチャンネルのログストリームを取得します。
	static LogStream Stream(LogLevel verbosity = LogLevel::Verbose);
	
	///@~Japanese
	/// @brief 指定されたチャンネルのログストリームを取得します。
	static LogStream Stream(std::string const& channelName, LogLevel verbosity = LogLevel::Verbose);
	
	///@~Japanese
	/// @brief 回復することが不可能なエラーが起きた場合に呼び出します。
	/// @param message エラーの詳細な内容
	static void Critical(std::string const& channel, std::string const& message);
	
	///@~Japanese
	/// @brief 深刻なエラーや未定義の動作を引き起こしかねない場合に呼び出します。
	/// @param message 警告の詳細な内容
	static void Warning(std::string const& channel, std::string const& message);

	///@~Japanese
	/// @brief デフォルトのチャンネルにログを出力します。
	static void Info(std::string const& message);
	
	///@~Japanese
	/// @brief 指定されたチャンネルにログを出力します。
	static void Info(std::string const& channel, std::string const& message);

	///@~Japanese
	/// @brief デフォルトのチャンネルにログを出力します。
	static void Verbose(std::string const& message);

	///@~Japanese
	/// @brief 指定されたチャンネルにログを出力します。
	static void Verbose(std::string const& channel, std::string const& message);
	
	///@~Japanese
	/// @brief デフォルトのチャンネルにログを出力します。
	static void Internal(std::string const& message);

	///@~Japanese
	/// @brief 指定されたチャンネルにログを出力します。
	static void Internal(std::string const& channel, std::string const& message);
};

}// namespace Pomdog

#endif // !defined(POMDOG_LOG_B5157F87_E1D1_460C_8346_161983EF05D9_HPP)
