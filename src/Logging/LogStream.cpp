//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Logging/LogStream.hpp>
#include <utility>
#include <Pomdog/Logging/LogChannel.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
LogStream::LogStream(LogChannel & channel, LoggingLevel verbosity)
	: channel(channel)
	, verbosity(verbosity)
{}
//-----------------------------------------------------------------------
LogStream::LogStream(LogStream const& other)
	: channel(other.channel)
	, verbosity(other.verbosity)
{
	cache << other.cache.str();
}
//-----------------------------------------------------------------------
LogStream::~LogStream()
{
	auto const messages = cache.str();
	if (!messages.empty())
	{
		channel.LogMessage(messages, verbosity);
	}
}
//-----------------------------------------------------------------------
LogStream & LogStream::operator<<(Flush const&)
{
	channel.LogMessage(cache.str(), verbosity);
	cache.str(std::string());
	cache.clear();
	return *this;
}

}// namespace Pomdog
