// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/tls_stream.h"
#include "pomdog/network/address_parser.h"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/network/emscripten/tls_stream_emscripten.h"
#else
#include "pomdog/network/mbedtls/certificates.h"
#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#endif

namespace pomdog {

TLSStream::TLSStream() = default;

TLSStream::TLSStream(IOService* service)
    : nativeStream_(std::make_unique<detail::NativeTLSStream>(service))
{
}

TLSStream::~TLSStream()
{
}

TLSStream::TLSStream(TLSStream&& other) = default;
TLSStream& TLSStream::operator=(TLSStream&& other) = default;

std::tuple<TLSStream, std::unique_ptr<Error>>
TLSStream::connect(IOService* service, std::string_view address)
{
    POMDOG_ASSERT(service != nullptr);

    TLSStream stream{service};
    POMDOG_ASSERT(stream.nativeStream_ != nullptr);

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    if (auto err = stream.nativeStream_->connect(host, port, std::chrono::seconds{5}); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#else
    const auto certPEM = detail::getEmbeddedCertificatePEM();

    if (auto err = stream.nativeStream_->connect(host, port, std::chrono::seconds{5}, certPEM); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#endif
    return std::make_tuple(std::move(stream), nullptr);
}

std::tuple<TLSStream, std::unique_ptr<Error>>
TLSStream::connect(
    IOService* service,
    std::string_view address,
    const Duration& timeout,
    [[maybe_unused]] const ArrayView<std::uint8_t const>& certPEM)
{
    POMDOG_ASSERT(service != nullptr);

    TLSStream stream{service};
    POMDOG_ASSERT(stream.nativeStream_ != nullptr);

    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    if (auto err = stream.nativeStream_->connect(host, port, timeout); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#else
    if (auto err = stream.nativeStream_->connect(host, port, timeout, certPEM); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#endif
    return std::make_tuple(std::move(stream), nullptr);
}

void TLSStream::disconnect()
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    nativeStream_->close();
}

std::unique_ptr<Error> TLSStream::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->write(data);
}

Connection TLSStream::onConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onConnected.connect(std::move(callback));
}

Connection TLSStream::onDisconnect(std::function<void()>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onDisconnect.connect(std::move(callback));
}

Connection TLSStream::onRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->onRead.connect(std::move(callback));
}

bool TLSStream::isConnected() const noexcept
{
    if (nativeStream_ == nullptr) {
        return false;
    }
    return nativeStream_->isConnected();
}

void TLSStream::setTimeout(const Duration& timeout)
{
    POMDOG_ASSERT(nativeStream_ != nullptr);
    return nativeStream_->setTimeout(timeout);
}

} // namespace pomdog
