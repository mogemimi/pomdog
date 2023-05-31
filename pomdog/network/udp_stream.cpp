// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/udp_stream.h"
#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/network/posix/udp_stream_posix.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/network/emscripten/udp_stream_emscripten.h"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/udp_stream_win32.h"
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

UDPStream::UDPStream() = default;

UDPStream::UDPStream(IOService* service)
    : nativeStream_(std::make_unique<detail::NativeUDPStream>(service))
{
}

UDPStream::~UDPStream()
{
}

UDPStream::UDPStream(UDPStream&& other) = default;

UDPStream& UDPStream::operator=(UDPStream&& other) = default;

std::tuple<UDPStream, std::unique_ptr<Error>>
UDPStream::connect(IOService* service, std::string_view address)
{
    POMDOG_ASSERT(service != nullptr);

    UDPStream stream{service};
    POMDOG_ASSERT(stream.nativeStream_ != nullptr);

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

    if (auto err = stream.nativeStream_->connect(host, port, std::chrono::seconds{5}); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
    return std::make_tuple(std::move(stream), nullptr);
}

std::tuple<UDPStream, std::unique_ptr<Error>>
UDPStream::listen(IOService* service, std::string_view address)
{
    POMDOG_ASSERT(service != nullptr);

    UDPStream stream{service};
    POMDOG_ASSERT(stream.nativeStream_ != nullptr);

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

    if (auto err = stream.nativeStream_->listen(host, port); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
    return std::make_tuple(std::move(stream), nullptr);
}

void UDPStream::disconnect()
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    nativeStream_->close();
}

std::unique_ptr<Error>
UDPStream::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->write(data);
}

std::unique_ptr<Error>
UDPStream::writeTo(const ArrayView<std::uint8_t const>& data, std::string_view address)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->writeTo(data, address);
}

Connection UDPStream::onConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onConnected.Connect(std::move(callback));
}

Connection UDPStream::onRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onRead.Connect(std::move(callback));
}

Connection UDPStream::onReadFrom(std::function<void(const ArrayView<std::uint8_t>&, std::string_view address, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onReadFrom.Connect(std::move(callback));
}

} // namespace pomdog
