// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOG_B5157F87_E1D1_460C_8346_161983EF05D9_HPP
#define POMDOG_LOG_B5157F87_E1D1_460C_8346_161983EF05D9_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <functional>

namespace Pomdog {

class EventConnection;
class LogEntry;
class LogStream;

class POMDOG_EXPORT Log {
public:
	static EventConnection Connect(std::function<void(LogEntry const&)> const& slot);

	static EventConnection Connect(std::function<void(LogEntry const&)> && slot);

	static EventConnection Connect(std::string const& channelName, std::function<void(LogEntry const&)> const& slot);

	static EventConnection Connect(std::string const& channelName, std::function<void(LogEntry const&)> && slot);

	static LogLevel GetLevel();

	static void SetLevel(LogLevel verbosity);

	static LogLevel GetLevel(std::string const& channelName);

	static void SetLevel(std::string const& channelName, LogLevel verbosity);

	static LogStream Stream(LogLevel verbosity = LogLevel::Verbose);

	static LogStream Stream(std::string const& channelName, LogLevel verbosity = LogLevel::Verbose);

	static void Critical(std::string const& channel, std::string const& message);

	static void Warning(std::string const& channel, std::string const& message);

	static void Info(std::string const& message);

	static void Info(std::string const& channel, std::string const& message);

	static void Verbose(std::string const& message);

	static void Verbose(std::string const& channel, std::string const& message);

	static void Internal(std::string const& message);

	static void Internal(std::string const& channel, std::string const& message);
};

}// namespace Pomdog

#endif // !defined(POMDOG_LOG_B5157F87_E1D1_460C_8346_161983EF05D9_HPP)
