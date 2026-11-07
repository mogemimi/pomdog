// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/trust_store.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: The browser verifies servers.
#else
#include "pomdog/network/mbedtls/trust_store_mbedtls.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {
namespace {

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
/// TrustStoreWeb is an empty trust store, because the browser verifies servers.
class TrustStoreWeb final : public TrustStore {};
#endif

} // namespace

TrustStore::TrustStore() noexcept = default;

TrustStore::~TrustStore() = default;

std::expected<std::shared_ptr<TrustStore>, std::unique_ptr<Error>>
createTrustStore([[maybe_unused]] const TrustStoreDesc& desc)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    return std::make_shared<TrustStoreWeb>();
#else
    auto store = std::make_shared<detail::TrustStoreMbedTLS>();
    if (auto result = store->initialize(desc); !result) {
        return std::unexpected(std::move(result.error()));
    }
    return store;
#endif
}

} // namespace pomdog::network
