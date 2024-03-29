// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/emscripten/tcp_stream_emscripten.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

TCPStreamEmscripten::TCPStreamEmscripten([[maybe_unused]] IOService* serviceIn)
{
}

TCPStreamEmscripten::~TCPStreamEmscripten()
{
}

void TCPStreamEmscripten::close()
{
    // FIXME: Not implemented yet.
}

std::unique_ptr<Error>
TCPStreamEmscripten::connect(
    [[maybe_unused]] std::string_view host,
    [[maybe_unused]] std::string_view port,
    [[maybe_unused]] const Duration& connectTimeout)
{
    // FIXME: Not implemented yet.
    return errors::make("not implemented yet");
}

std::unique_ptr<Error>
TCPStreamEmscripten::write([[maybe_unused]] const ArrayView<std::uint8_t const>& data)
{
    // FIXME: Not implemented yet.
    return errors::make("not implemented yet");
}

bool TCPStreamEmscripten::isConnected() const noexcept
{
    // FIXME: Not implemented yet.
    return false;
}

void TCPStreamEmscripten::setTimeout([[maybe_unused]] const Duration& timeoutIn)
{
    // FIXME: Not implemented yet.
}

} // namespace pomdog::detail
