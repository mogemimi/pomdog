// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOGSTREAM_9EF46BAA_HPP
#define POMDOG_LOGSTREAM_9EF46BAA_HPP

#include "LogLevel.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <type_traits>
#include <string>
#include <sstream>
#include <functional>

namespace Pomdog {

class LogChannel;

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


namespace Detail {
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
}// namespace Detail

template <typename T>
LogStream & LogStream::operator<<(T const& message)
{
    cache << Detail::Logging::LogStreamInvork(message);
    return *this;
}

}// namespace Pomdog

#endif // POMDOG_LOGSTREAM_9EF46BAA_HPP
