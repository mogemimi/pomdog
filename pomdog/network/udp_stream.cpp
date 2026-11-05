// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/udp_stream.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/host_port.h"

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/network/posix/udp_stream_posix.h"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/udp_stream_win32.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: The web has no UDP sockets.
#else
#error "Platform undefined or not supported."
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {
namespace {

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
using NativeUDPStream = detail::UDPStreamPOSIX;
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
using NativeUDPStream = detail::UDPStreamWin32;
#endif

} // namespace

UDPStream::UDPStream() noexcept = default;

UDPStream::~UDPStream() = default;

std::expected<std::shared_ptr<UDPStream>, std::unique_ptr<Error>>
connectUDPStream([[maybe_unused]] std::string_view address)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::unexpected(errors::make("UDP streams are not supported on the web"));
#else
    const auto [host, port] = detail::splitHostPort(address);

    auto stream = std::make_shared<NativeUDPStream>();
    if (auto result = stream->connect(host, port); !result) {
        return std::unexpected(std::move(result.error()));
    }
    return stream;
#endif
}

std::expected<std::shared_ptr<UDPStream>, std::unique_ptr<Error>>
listenUDPStream([[maybe_unused]] std::string_view address)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::unexpected(errors::make("UDP streams are not supported on the web"));
#else
    const auto [host, port] = detail::splitHostPort(address);

    auto stream = std::make_shared<NativeUDPStream>();
    if (auto result = stream->listen(host, port); !result) {
        return std::unexpected(std::move(result.error()));
    }
    return stream;
#endif
}

} // namespace pomdog::network
