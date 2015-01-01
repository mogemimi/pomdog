//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_LOGSTREAM_9EF46BAA_9822_46E4_BCC6_DC71864ADAE5_HPP
#define POMDOG_LOGSTREAM_9EF46BAA_9822_46E4_BCC6_DC71864ADAE5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <type_traits>
#include <string>
#include <sstream>
#include <functional>

namespace Pomdog {

class LogChannel;

///@~Japanese
/// @brief ログストリームです。
class POMDOG_EXPORT LogStream {
private:
	std::stringstream cache;
	std::string sourceChannel;
	LogChannel & channel;
	LogLevel level;

public:
	LogStream() = delete;
	
	explicit LogStream(LogChannel & channel, LogLevel level = LogLevel::Verbose);

	LogStream(LogChannel & channel, std::string const& sourceChannel, LogLevel level = LogLevel::Verbose);

	LogStream(LogStream const&);
	LogStream(LogStream &&) = default;
	LogStream & operator=(LogStream const&) = delete;
	LogStream & operator=(LogStream &&) = delete;
	
	~LogStream();

	template <typename T>
	LogStream & operator<<(T const& message);
	
	void Flush();
	
	void Clear();
	
	std::string String() const;
};


namespace Details {
namespace Logging {

template<typename E>
auto ToIntegral(E e)->typename std::underlying_type<E>::type
{
	return static_cast<typename std::underlying_type<E>::type>(e);
}

template <typename T>
auto LogStreamInvork(T const& message, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr)->decltype(ToIntegral(message))
{
	return ToIntegral(message);
}

template <typename T>
T const& LogStreamInvork(T const& message, typename std::enable_if<!std::is_enum<T>::value>::type* = nullptr)
{
	return message;
}

}// namespace Logging
}// namespace Details

template <typename T>
LogStream & LogStream::operator<<(T const& message)
{
	cache << Details::Logging::LogStreamInvork(message);
	return *this;
}

}// namespace Pomdog

#endif // !defined(POMDOG_LOGSTREAM_9EF46BAA_9822_46E4_BCC6_DC71864ADAE5_HPP)
