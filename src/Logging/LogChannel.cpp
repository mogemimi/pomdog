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

namespace Pomdog {
//-----------------------------------------------------------------------
LogChannel::LogChannel(std::string const& channnelName)
	: name(channnelName)
	, threshold(LogLevel::Verbose)
{}
//-----------------------------------------------------------------------
void LogChannel::LogMessage(std::string const& message, LogLevel verbosity)
{
	if (verbosity <= this->threshold)
	{
		eventHandler.Invoke<LogEntry>(message, name, verbosity);
	}
}
//-----------------------------------------------------------------------
EventConnection LogChannel::Connect(std::function<void(LogEntry const& log)> const& slot)
{
	POMDOG_ASSERT(slot);

	return eventHandler.Connect([slot](Event const& event)
	{
		if (auto log = event.As<LogEntry>()) {
			slot(*log);
		}
	});
}
//-----------------------------------------------------------------------
std::string const& LogChannel::Name() const
{
	return name;
}
//-----------------------------------------------------------------------
LogLevel LogChannel::Level() const
{
	return this->threshold;
}
//-----------------------------------------------------------------------
void LogChannel::Level(LogLevel thresholdIn)
{
	this->threshold = thresholdIn;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
