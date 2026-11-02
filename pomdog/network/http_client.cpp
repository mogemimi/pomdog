// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_client.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: The browser client is not implemented yet.
#else
#include "pomdog/network/native/http_client_native.h"
#endif

namespace pomdog::network {

HTTPClient::HTTPClient() noexcept = default;

HTTPClient::~HTTPClient() = default;

std::expected<std::shared_ptr<HTTPClient>, std::unique_ptr<Error>>
createHTTPClient([[maybe_unused]] const HTTPClientOptions& options)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::unexpected(errors::make("HTTPClient is not implemented on the web yet"));
#else
    return std::make_shared<detail::HTTPClientNative>(options);
#endif
}

} // namespace pomdog::network
