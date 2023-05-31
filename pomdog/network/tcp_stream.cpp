// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/tcp_stream.h"
#include "pomdog/basic/platform.h"
#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/network/posix/tcp_stream_posix.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/network/emscripten/tcp_stream_emscripten.h"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/tcp_stream_win32.h"
#else
#error "Platform undefined or not supported."
#endif
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/address_parser.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

TCPStream::TCPStream() = default;

TCPStream::TCPStream(IOService* service)
    : nativeStream_(std::make_unique<detail::NativeTCPStream>(service))
{
}

TCPStream::~TCPStream()
{
}

TCPStream::TCPStream(TCPStream&& other) = default;
TCPStream& TCPStream::operator=(TCPStream&& other) = default;

std::tuple<TCPStream, std::unique_ptr<Error>>
TCPStream::connect(IOService* service, std::string_view address)
{
    POMDOG_ASSERT(service != nullptr);

    TCPStream stream{service};
    POMDOG_ASSERT(stream.nativeStream_ != nullptr);

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

    if (auto err = stream.nativeStream_->connect(host, port, std::chrono::seconds{5}); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
    return std::make_tuple(std::move(stream), nullptr);
}

std::tuple<TCPStream, std::unique_ptr<Error>>
TCPStream::connect(IOService* service, std::string_view address, const Duration& timeout)
{
    POMDOG_ASSERT(service != nullptr);

    TCPStream stream{service};
    POMDOG_ASSERT(stream.nativeStream_ != nullptr);

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

    if (auto err = stream.nativeStream_->connect(host, port, timeout); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
    return std::make_tuple(std::move(stream), nullptr);
}

void TCPStream::disconnect()
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    nativeStream_->close();
}

Connection TCPStream::onConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onConnected.Connect(std::move(callback));
}

Connection TCPStream::onDisconnect(std::function<void()>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onDisconnect.Connect(std::move(callback));
}

Connection TCPStream::onRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onRead.Connect(std::move(callback));
}

std::unique_ptr<Error> TCPStream::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->write(data);
}

bool TCPStream::isConnected() const noexcept
{
    if (nativeStream_ == nullptr) {
        return false;
    }
    return nativeStream_->isConnected();
}

void TCPStream::setTimeout(const Duration& timeout)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    nativeStream_->setTimeout(timeout);
}

} // namespace pomdog
