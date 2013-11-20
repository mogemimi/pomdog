//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Logging/Logging.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Event/EventConnection.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>

namespace Pomdog {

class LogManager final
{
public:
	LogManager() = default;
	LogManager(LogManager const&) = delete;
	LogManager(LogManager &&) = delete;
	~LogManager() = default;
	
	LogManager & operator=(LogManager const&) = delete;
	LogManager & operator=(LogManager &&) = delete;

	LogChannel & FindChannel(std::string const& name)
	{
		auto iter = std::find_if(std::begin(channels), std::end(channels),
			[&name](std::unique_ptr<LogChannel> const& channel) {
				return name == channel->GetName();
		});

		if (std::end(channels) != iter) {
			return *(*iter);
		}
		return GetDefault();
	}
	
	bool HasChannel(std::string const& name) const
	{
		auto iter = std::find_if(std::begin(channels), std::end(channels),
			[&name](std::unique_ptr<LogChannel> const& channel) {
				return name == channel->GetName();
		});

		return (std::end(channels) != iter);
	}
	
	void AddChannel(std::string const& name, LoggingLevel verbosity)
	{
		if (HasChannel(name)) {
			return;
		}

		std::lock_guard<std::recursive_mutex> lock(channelsProtection);
		
		auto channel = std::unique_ptr<LogChannel>(new LogChannel(name));
		channel->SetLevel(verbosity);
		channels.push_back(std::move(channel));
	}
	
	void RemoveChannelByName(std::string const& name)
	{
		std::lock_guard<std::recursive_mutex> lock(channelsProtection);
	
		channels.erase(std::remove_if(std::begin(channels), std::end(channels),
			[&name](std::unique_ptr<LogChannel> const& channel) {
				return name == channel->GetName();
		}), std::end(channels));
	}
		
	LogChannel & GetDefault()
	{
		return defaultChannel;
	}

private:
	std::vector<std::unique_ptr<LogChannel>> channels;
	ScopedConnection<EventConnection> connection;
	std::recursive_mutex channelsProtection;
	
	static LogChannel defaultChannel;
	
public:
	static LogManager & GetInstance()
	{
		static LogManager manager;
		return manager;
	}
};

LogChannel LogManager::defaultChannel("");

//-----------------------------------------------------------------------
void Log::LogMessage(std::string const& message, LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	channel.LogMessage(message, verbosity);
}
//-----------------------------------------------------------------------
void Log::LogMessage(std::string const& message, std::string const& channelName, LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.FindChannel(channelName);
	channel.LogMessage(message, verbosity);
}
//-----------------------------------------------------------------------
LogStream Log::Stream(LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	return LogStream(manager.GetDefault(), verbosity);
}
//-----------------------------------------------------------------------
LogStream Log::Stream(std::string const& channelName, LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	return LogStream(manager.FindChannel(channelName), verbosity);
}
//-----------------------------------------------------------------------
void Log::AddChannel(std::string const& channelName, LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	manager.AddChannel(channelName, verbosity);
}
//-----------------------------------------------------------------------
void Log::RemoveChannel(std::string const& channelName)
{
	auto& manager = LogManager::GetInstance();
	manager.RemoveChannelByName(channelName);
}
//-----------------------------------------------------------------------
bool Log::ExistChannel(std::string const& channelName)
{
	auto& manager = LogManager::GetInstance();
	return manager.HasChannel(channelName);
}
//-----------------------------------------------------------------------
LoggingLevel Log::GetVerbosity()
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	return channel.GetLevel();
}
//-----------------------------------------------------------------------
void Log::SetVerbosity(LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	return channel.SetLevel(verbosity);
}
//-----------------------------------------------------------------------
LoggingLevel Log::GetVerbosity(std::string const& channelName)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.FindChannel(channelName);
	return channel.GetLevel();
}
//-----------------------------------------------------------------------
void Log::SetVerbosity(std::string const& channelName, LoggingLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	if (manager.HasChannel(channelName)) {
		auto& channel = manager.FindChannel(channelName);
		channel.SetLevel(verbosity);
	}
}
//-----------------------------------------------------------------------
EventConnection Log::Connect(std::function<void(LogEntry const&)> callback)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	return channel.Connect(callback);
}
//-----------------------------------------------------------------------
EventConnection Log::Connect(std::function<void(LogEntry const&)> callback, std::string const& channelName)
{
	auto& manager = LogManager::GetInstance();
	if (!manager.HasChannel(channelName)) {
		manager.AddChannel(channelName, LoggingLevel::Verbose);
	}
	auto& channel = manager.FindChannel(channelName);
	return channel.Connect(callback);
}

}// namespace Pomdog
