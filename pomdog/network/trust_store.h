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

/// TrustStore holds the root certificates that TLS connections trust.
///
/// A TLS stream, and an `HTTPClient` for https requests, accepts a server only
/// when its certificate chains up to one of these roots and names the host
/// that it connects to. Pomdog has no built-in roots, so the application
/// creates a store from the roots it ships, such as
/// `assets/certificates/root_certificates.pem`. A store does not change after
/// it is created, so streams and clients can share it.
///
/// On the web the browser verifies servers, and the store holds nothing.
class POMDOG_EXPORT TrustStore {
public:
    TrustStore() noexcept;

    TrustStore(const TrustStore&) = delete;
    TrustStore& operator=(const TrustStore&) = delete;

    virtual ~TrustStore();
};

/// Creates a trust store from the root certificates in pem, which holds one or
/// more certificates in PEM format. Text between the certificates, such as
/// comments, is ignored.
///
/// Returns an error when pem holds no certificate, or a certificate that
/// cannot be parsed. On the web it does not read pem and always succeeds.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<TrustStore>, std::unique_ptr<Error>>
createTrustStoreFromPEM(std::span<const u8> pem);

} // namespace pomdog::network
