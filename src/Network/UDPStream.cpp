// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Network/UDPStream.hpp"
#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
#include "../Network.POSIX/UDPStreamPOSIX.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../Network.Win32/UDPStreamWin32.hpp"
#endif
#include "AddressParser.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

UDPStream::UDPStream() = default;

UDPStream::UDPStream(IOService* service)
    : nativeStream(std::make_unique<Detail::NativeUDPStream>(service))
{
}

UDPStream::~UDPStream()
{
}

UDPStream::UDPStream(UDPStream&& other) = default;
UDPStream& UDPStream::operator=(UDPStream&& other) = default;

std::tuple<UDPStream, std::shared_ptr<Error>>
UDPStream::Connect(IOService* service, const std::string_view& address)
{
    POMDOG_ASSERT(service != nullptr);

    UDPStream stream{service};
    POMDOG_ASSERT(stream.nativeStream != nullptr);

    const auto [family, host, port] = Detail::AddressParser::TransformAddress(address);

    if (auto err = stream.nativeStream->Connect(host, port, std::chrono::seconds{5}); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
    return std::make_tuple(std::move(stream), nullptr);
}

std::tuple<UDPStream, std::shared_ptr<Error>>
UDPStream::Listen(IOService* service, const std::string_view& address)
{
    POMDOG_ASSERT(service != nullptr);

    UDPStream stream{service};
    POMDOG_ASSERT(stream.nativeStream != nullptr);

    const auto [family, host, port] = Detail::AddressParser::TransformAddress(address);

    if (auto err = stream.nativeStream->Listen(host, port); err != nullptr) {
        return std::make_tuple(std::move(stream), std::move(err));
    }
    return std::make_tuple(std::move(stream), nullptr);
}

void UDPStream::Disconnect()
{
    POMDOG_ASSERT(nativeStream != nullptr);
    nativeStream->Close();
}

std::shared_ptr<Error>
UDPStream::Write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->Write(data);
}

std::shared_ptr<Error>
UDPStream::WriteTo(const ArrayView<std::uint8_t const>& data, const std::string_view& address)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->WriteTo(data, address);
}

Connection UDPStream::OnConnected(std::function<void(const std::shared_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->OnConnected.Connect(std::move(callback));
}

Connection UDPStream::OnRead(std::function<void(const ArrayView<std::uint8_t>&, const std::shared_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->OnRead.Connect(std::move(callback));
}

Connection UDPStream::OnReadFrom(std::function<void(const ArrayView<std::uint8_t>&, const std::string_view& address, const std::shared_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(nativeStream != nullptr);
    return nativeStream->OnReadFrom.Connect(std::move(callback));
}

} // namespace Pomdog
