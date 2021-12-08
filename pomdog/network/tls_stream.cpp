// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/tls_stream.hpp"
#include "pomdog/network/address_parser.hpp"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/network/emscripten/tls_stream_emscripten.hpp"
#else
#include "pomdog/network/mbedtls/certificates.hpp"
#include "pomdog/network/mbedtls/tls_stream_mbedtls.hpp"
#endif

namespace pomdog {

TLSStream::TLSStream() = default;

TLSStream::TLSStream(IOService* service)
    : nativeStream(std::make_unique<detail::NativeTLSStream>(service))
{
}

TLSStream::~TLSStream()
{
}

TLSStream::TLSStream(TLSStream&& other) = default;
TLSStream& TLSStream::operator=(TLSStream&& other) = default;

std::tuple<TLSStream, std::unique_ptr<Error>>
TLSStream::Connect(IOService* service, std::string_view address)
{
    POMDOG_ASSERT(service != nullptr);

    TLSStream stream{service};
    POMDOG_ASSERT(stream.nativeStream != nullptr);

    const auto [family, host, port] = detail::AddressParser::TransformAddress(address);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    if (auto err = stream.nativeStream->Connect(host, port, std::chrono::seconds{5}); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#else
    const auto certPEM = detail::GetEmbeddedCertificatePEM();

    if (auto err = stream.nativeStream->Connect(host, port, std::chrono::seconds{5}, certPEM); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#endif
    return std::make_tuple(std::move(stream), nullptr);
}

std::tuple<TLSStream, std::unique_ptr<Error>>
TLSStream::Connect(
    IOService* service,
    std::string_view address,
    const Duration& timeout,
    [[maybe_unused]] const ArrayView<std::uint8_t const>& certPEM)
{
    POMDOG_ASSERT(service != nullptr);

    TLSStream stream{service};
    POMDOG_ASSERT(stream.nativeStream != nullptr);

    const auto [family, host, port] = detail::AddressParser::TransformAddress(address);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    if (auto err = stream.nativeStream->Connect(host, port, timeout); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#else
    if (auto err = stream.nativeStream->Connect(host, port, timeout, certPEM); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#endif
    return std::make_tuple(std::move(stream), nullptr);
}

void TLSStream::Disconnect()
{
    POMDOG_ASSERT(nativeStream != nullptr);
    nativeStream->Close();
}

std::unique_ptr<Error> TLSStream::Write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->Write(data);
}

Connection TLSStream::OnConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->OnConnected.Connect(std::move(callback));
}

Connection TLSStream::OnDisconnect(std::function<void()>&& callback)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->OnDisconnect.Connect(std::move(callback));
}

Connection TLSStream::OnRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->OnRead.Connect(std::move(callback));
}

bool TLSStream::IsConnected() const noexcept
{
    if (nativeStream == nullptr) {
        return false;
    }
    return nativeStream->IsConnected();
}

void TLSStream::SetTimeout(const Duration& timeout)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->SetTimeout(timeout);
}

} // namespace pomdog
