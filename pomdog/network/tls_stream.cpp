// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/tls_stream.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: The web has no TCP sockets.
#else
#include "pomdog/network/address_parser.h"
#include "pomdog/network/mbedtls/certificates.h"
#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

std::expected<std::shared_ptr<Stream>, std::unique_ptr<Error>>
connectTLSStream(
    [[maybe_unused]] std::string_view address,
    [[maybe_unused]] const StreamOptions& options)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::unexpected(errors::make("TLS streams are not supported on the web"));
#else
    const auto [family, host, port] = detail::AddressParser::transformAddress(address);

    auto stream = std::make_shared<detail::TLSStreamMbedTLS>();
    if (auto result = stream->connect(host, port, options, detail::getEmbeddedCertificatePEM()); !result) {
        return std::unexpected(std::move(result.error()));
    }
    return stream;
#endif
}

} // namespace pomdog::network
