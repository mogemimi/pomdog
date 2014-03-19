//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Logging/Log.hpp>
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>
#include <Pomdog/Logging/LogEntry.hpp>
#include <Pomdog/Logging/LogStream.hpp>
#include <Pomdog/Logging/LogChannel.hpp>
#include <Pomdog/Event/ScopedConnection.hpp>

namespace Pomdog {
namespace {

//namespace Drafts {
//
//class Logger
//{
//public:
//	///@~Japanese
//	/// @param message メッセージの内容です。
//	static void Info(std::string const& message);
//
//	///@~Japanese
//	/// @brief 深刻なエラーや未定義の動作を引き起こしかねない場合に、警告の通知を受け取ります。
//	/// @param message 警告の詳細な内容です。
//	static void Warning(std::string const& message);
//	
//	///@~Japanese
//	/// @brief 回復することが不可能なエラーが起きた場合、呼び出します。
//	/// @param message エラーの詳細なメッセージを渡します。
//	static void FatalError(std::string const& message);
//};
//
////-----------------------------------------------------------------------
//void Logger::Info(std::string const& message)
//{
//	Log::LogMessage(message, LogLevel::Brief);
//}
////-----------------------------------------------------------------------
//void Logger::Warning(std::string const& message)
//{
//	Log::LogMessage(message, LogLevel::Brief);
//}
////-----------------------------------------------------------------------
//void Logger::FatalError(std::string const& message)
//{
//	Log::LogMessage(message, LogLevel::Critical);
//}
//
//}// namespace Drafts

class LogManager final {
public:
	LogManager()
		:defaultChannel("")
	{}
	
	LogManager(LogManager const&) = delete;
	LogManager(LogManager &&) = delete;
	~LogManager() = default;
	
	LogManager & operator=(LogManager const&) = delete;
	LogManager & operator=(LogManager &&) = delete;

	LogChannel & FindChannel(std::string const& name)
	{
		auto iter = std::find_if(std::begin(channels), std::end(channels),
			[&name](std::unique_ptr<LogChannel> const& channel) {
				return name == channel->Name();
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
				return name == channel->Name();
		});

		return (std::end(channels) != iter);
	}
	
	void AddChannel(std::string const& name, LogLevel verbosity)
	{
		if (HasChannel(name)) {
			return;
		}

		std::lock_guard<std::recursive_mutex> lock(channelsProtection);
		
		auto channel = std::unique_ptr<LogChannel>(new LogChannel(name));
		channel->Level(verbosity);
		channels.push_back(std::move(channel));
	}
	
	void RemoveChannelByName(std::string const& name)
	{
		std::lock_guard<std::recursive_mutex> lock(channelsProtection);
	
		channels.erase(std::remove_if(std::begin(channels), std::end(channels),
			[&name](std::unique_ptr<LogChannel> const& channel) {
				return name == channel->Name();
		}), std::end(channels));
	}

	LogChannel & GetDefault()
	{
		return defaultChannel;
	}

private:
	std::vector<std::unique_ptr<LogChannel>> channels;
	ScopedConnection connection;
	std::recursive_mutex channelsProtection;

	LogChannel defaultChannel;
	
public:
	static LogManager & GetInstance()
	{
		static LogManager manager;
		return manager;
	}
};

}// unnamed namespace
//-----------------------------------------------------------------------
void Log::LogMessage(std::string const& message, LogLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	channel.LogMessage(message, verbosity);
}
//-----------------------------------------------------------------------
void Log::LogMessage(std::string const& message, std::string const& channelName, LogLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.FindChannel(channelName);
	channel.LogMessage(message, verbosity);
}
//-----------------------------------------------------------------------
LogStream Log::Stream(LogLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	return LogStream(manager.GetDefault(), verbosity);
}
//-----------------------------------------------------------------------
LogStream Log::Stream(std::string const& channelName, LogLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	return LogStream(manager.FindChannel(channelName), verbosity);
}
//-----------------------------------------------------------------------
void Log::AddChannel(std::string const& channelName, LogLevel verbosity)
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
LogLevel Log::GetVerbosity()
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	return channel.Level();
}
//-----------------------------------------------------------------------
void Log::SetVerbosity(LogLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.GetDefault();
	channel.Level(verbosity);
}
//-----------------------------------------------------------------------
LogLevel Log::GetVerbosity(std::string const& channelName)
{
	auto& manager = LogManager::GetInstance();
	auto& channel = manager.FindChannel(channelName);
	return channel.Level();
}
//-----------------------------------------------------------------------
void Log::SetVerbosity(std::string const& channelName, LogLevel verbosity)
{
	auto& manager = LogManager::GetInstance();
	if (manager.HasChannel(channelName)) {
		auto& channel = manager.FindChannel(channelName);
		channel.Level(verbosity);
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
		manager.AddChannel(channelName, LogLevel::Verbose);
	}
	auto& channel = manager.FindChannel(channelName);
	return channel.Connect(callback);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
