// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)

class TCPListenerPOSIX;
class TCPStreamPOSIX;
class UDPStreamPOSIX;

using NativeTCPListener = TCPListenerPOSIX;
using NativeTCPStream = TCPStreamPOSIX;
using NativeUDPStream = UDPStreamPOSIX;

#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)

class TCPListenerWin32;
class TCPStreamWin32;
class UDPStreamWin32;

using NativeTCPListener = TCPListenerWin32;
using NativeTCPStream = TCPStreamWin32;
using NativeUDPStream = UDPStreamWin32;

#else
#error "Platform undefined or not supported."
#endif

class TLSStreamMbedTLS;
using NativeTLSStream = TLSStreamMbedTLS;

} // namespace Pomdog::Detail

namespace Pomdog {

class HTTPClient;
class HTTPRequest;
class HTTPResponse;
class IOService;
class TCPListener;
class TCPStream;
class UDPStream;

template <typename T>
class ArrayView;

enum class HTTPMethod : std::int8_t;

} // namespace Pomdog
