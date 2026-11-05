// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/tls_stream.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: The web has no TCP sockets.
#else
#include "pomdog/network/host_port.h"
#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#include "pomdog/network/mbedtls/trust_store_mbedtls.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

std::expected<std::shared_ptr<Stream>, std::unique_ptr<Error>>
connectTLSStream(
    [[maybe_unused]] std::string_view address,
    [[maybe_unused]] const std::shared_ptr<TrustStore>& trustStore,
    [[maybe_unused]] const StreamOptions& options)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::unexpected(errors::make("TLS streams are not supported on the web"));
#else
    if (trustStore == nullptr) {
        return std::unexpected(errors::make("a TLS stream needs a trust store"));
    }
    auto store = std::dynamic_pointer_cast<detail::TrustStoreMbedTLS>(trustStore);
    if (store == nullptr) {
        return std::unexpected(errors::make("the trust store must come from createTrustStoreFromPEM()"));
    }

    const auto [host, port] = detail::splitHostPort(address);

    auto stream = std::make_shared<detail::TLSStreamMbedTLS>();
    if (auto result = stream->connect(host, port, options, std::move(store)); !result) {
        return std::unexpected(std::move(result.error()));
    }
    return stream;
#endif
}

} // namespace pomdog::network
