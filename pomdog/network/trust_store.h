// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

/// TrustStore holds the root certificates that TLS connections trust, and the
/// public keys that they distrust.
///
/// A TLS stream, and an `HTTPClient` for https requests, accepts a server only
/// when its certificate chains up to one of the roots and names the host that
/// it connects to, and when no certificate in the chain has a distrusted key.
/// Pomdog has no built-in roots, so the application creates a store from the
/// roots it ships, such as `assets/certificates/root_certificates.pem`. A store
/// does not change after it is created, so streams and clients can share it.
///
/// On the web the browser verifies servers, and the store holds nothing.
class POMDOG_EXPORT TrustStore {
public:
    TrustStore() noexcept;

    TrustStore(const TrustStore&) = delete;
    TrustStore& operator=(const TrustStore&) = delete;

    virtual ~TrustStore();
};

/// Describes the data of a trust store. The store copies what it needs, so the
/// data needs to stay valid only during `createTrustStore()`.
struct TrustStoreDesc final {
    /// The root certificates, one or more in PEM format. Text between the
    /// certificates, such as comments, is ignored.
    std::span<const u8> rootCertificatesPEM;

    /// The distrusted public keys, as a `.dkeys` file that distrusted-keys-gen
    /// writes from a PEM file of distrusted certificates, such as
    /// `assets/certificates/distrusted_certificates.pem`. Empty for none.
    std::span<const u8> distrustedKeys;
};

/// Creates a trust store from desc.
///
/// Returns an error when the roots hold no certificate or one that cannot be
/// parsed, when the distrusted keys are not a valid `.dkeys` file, and when a
/// root has a distrusted key. On the web it reads neither and always succeeds.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<TrustStore>, std::unique_ptr<Error>>
createTrustStore(const TrustStoreDesc& desc);

} // namespace pomdog::network
