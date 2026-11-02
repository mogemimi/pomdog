// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/tcp_stream.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/address_parser.h"

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/network/posix/tcp_stream_posix.h"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/tcp_stream_win32.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: The web has no TCP sockets.
#else
#error "Platform undefined or not supported."
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

std::expected<std::shared_ptr<Stream>, std::unique_ptr<Error>>
connectTCPStream(
    [[maybe_unused]] std::string_view address,
    [[maybe_unused]] const StreamOptions& options)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::unexpected(errors::make("TCP streams are not supported on the web"));
#else
#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
    using NativeStream = detail::TCPStreamPOSIX;
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    using NativeStream = detail::TCPStreamWin32;
#endif

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

    auto stream = std::make_shared<NativeStream>();
    if (auto result = stream->connect(host, port, options); !result) {
        return std::unexpected(std::move(result.error()));
    }
    return stream;
#endif
}

} // namespace pomdog::network
