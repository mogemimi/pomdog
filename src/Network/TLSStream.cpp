// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Network/TLSStream.hpp"
#include "AddressParser.hpp"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "../Network.Emscripten/TLSStreamEmscripten.hpp"
#else
#include "../Network.MbedTLS/Certificates.hpp"
#include "../Network.MbedTLS/TLSStreamMbedTLS.hpp"
#endif

namespace Pomdog {

TLSStream::TLSStream() = default;

TLSStream::TLSStream(IOService* service)
    : nativeStream(std::make_unique<Detail::NativeTLSStream>(service))
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

    const auto [family, host, port] = Detail::AddressParser::TransformAddress(address);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    if (auto err = stream.nativeStream->Connect(host, port, std::chrono::seconds{5}); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
#else
    const auto certPEM = Detail::GetEmbeddedCertificatePEM();

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

    const auto [family, host, port] = Detail::AddressParser::TransformAddress(address);

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

} // namespace Pomdog
