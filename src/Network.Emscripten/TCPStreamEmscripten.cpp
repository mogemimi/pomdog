// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "TCPStreamEmscripten.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

TCPStreamEmscripten::TCPStreamEmscripten([[maybe_unused]] IOService* serviceIn)
{
}

TCPStreamEmscripten::~TCPStreamEmscripten()
{
}

void TCPStreamEmscripten::Close()
{
    // FIXME: Not implemented yet.
}

std::unique_ptr<Error>
TCPStreamEmscripten::Connect(
    [[maybe_unused]] std::string_view host,
    [[maybe_unused]] std::string_view port,
    [[maybe_unused]] const Duration& connectTimeout)
{
    // FIXME: Not implemented yet.
    return Errors::New("not implemented yet");
}

std::unique_ptr<Error>
TCPStreamEmscripten::Write([[maybe_unused]] const ArrayView<std::uint8_t const>& data)
{
    // FIXME: Not implemented yet.
    return Errors::New("not implemented yet");
}

bool TCPStreamEmscripten::IsConnected() const noexcept
{
    // FIXME: Not implemented yet.
    return false;
}

void TCPStreamEmscripten::SetTimeout([[maybe_unused]] const Duration& timeoutIn)
{
    // FIXME: Not implemented yet.
}

} // namespace Pomdog::Detail
