//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Event/Event.hpp>
#include <Pomdog/Event/EventConnection.hpp>
#include <Pomdog/Logging/LogEntry.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
LogChannel::LogChannel(std::string const& channnelName)
	: name(channnelName)
	, level(LogLevel::Verbose)
{}
//-----------------------------------------------------------------------
void LogChannel::Log(std::string const& message, LogLevel verbosity)
{
	if (verbosity <= this->level) {
		signal(LogEntry{message, name, verbosity});
	}
}
//-----------------------------------------------------------------------
void LogChannel::Log(LogEntry const& logEntry)
{
	if (logEntry.Verbosity <= this->level) {
		signal(logEntry);
	}
}
//-----------------------------------------------------------------------
EventConnection LogChannel::Connect(std::function<void(LogEntry const&)> const& slot)
{
	POMDOG_ASSERT(slot);
	return signal.Connect(slot);
}
//-----------------------------------------------------------------------
EventConnection LogChannel::Connect(std::function<void(LogEntry const&)> && slot)
{
	POMDOG_ASSERT(slot);
	return signal.Connect(std::move(slot));
}
//-----------------------------------------------------------------------
std::string const& LogChannel::Name() const
{
	return name;
}
//-----------------------------------------------------------------------
LogLevel LogChannel::Level() const
{
	return this->level;
}
//-----------------------------------------------------------------------
void LogChannel::Level(LogLevel levelIn)
{
	this->level = levelIn;
}
//-----------------------------------------------------------------------
std::size_t LogChannel::ConnectionCount() const
{
	return signal.InvocationCount();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
