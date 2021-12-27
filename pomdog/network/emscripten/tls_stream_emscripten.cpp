// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/emscripten/tls_stream_emscripten.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

TLSStreamEmscripten::TLSStreamEmscripten([[maybe_unused]] IOService* serviceIn)
{
}

TLSStreamEmscripten::~TLSStreamEmscripten()
{
}

std::unique_ptr<Error>
TLSStreamEmscripten::Connect(
    [[maybe_unused]] std::string_view host,
    [[maybe_unused]] std::string_view port,
    [[maybe_unused]] const Duration& connectTimeoutIn)
{
    // FIXME: Not implemented yet.
    return errors::New("not implemented yet");
}

void TLSStreamEmscripten::Close()
{
    // FIXME: Not implemented yet.
}

std::unique_ptr<Error>
TLSStreamEmscripten::Write([[maybe_unused]] const ArrayView<std::uint8_t const>& data)
{
    // FIXME: Not implemented yet.
    return errors::New("not implemented yet");
}

bool TLSStreamEmscripten::IsConnected() const noexcept
{
    // FIXME: Not implemented yet.
    return false;
}

void TLSStreamEmscripten::SetTimeout([[maybe_unused]] const Duration& timeoutIn)
{
    // FIXME: Not implemented yet.
}

} // namespace pomdog::detail
