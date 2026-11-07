// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/mbedtls/mbedtls_helper.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <vector>

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4061)
POMDOG_CLANG_SUPPRESS_WARNING_PUSH
POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation")
POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation-unknown-command")
POMDOG_CLANG_SUPPRESS_WARNING("-Wold-style-cast")
POMDOG_CLANG_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
POMDOG_GCC_SUPPRESS_WARNING_PUSH
POMDOG_GCC_SUPPRESS_WARNING("-Wredundant-decls")
POMDOG_GCC_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
#include "mbedtls/x509_crt.h"
POMDOG_GCC_SUPPRESS_WARNING_POP
POMDOG_CLANG_SUPPRESS_WARNING_POP
POMDOG_MSVC_SUPPRESS_WARNING_POP

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// TrustStoreMbedTLS holds root certificates as an Mbed TLS certificate chain,
/// and the sorted SHA-256 hashes of the distrusted public keys.
class TrustStoreMbedTLS final : public TrustStore {
public:
    TrustStoreMbedTLS() noexcept;

    ~TrustStoreMbedTLS() override;

    /// Parses the root certificates and copies the distrusted keys of desc.
    /// Fails unless every certificate parses, the keys are a valid sorted
    /// `.dkeys` file, and no root has a distrusted key.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    initialize(const TrustStoreDesc& desc);

    /// Returns the certificate chain, which TLS streams verify servers against.
    [[nodiscard]] ::mbedtls_x509_crt*
    getCertificates() noexcept;

    /// Returns true if the store distrusts any key.
    [[nodiscard]] bool
    hasDistrustedKeys() const noexcept;

    /// Returns true if the public key of certificate is distrusted, or if its
    /// hash cannot be computed.
    [[nodiscard]] bool
    isDistrusted(const ::mbedtls_x509_crt& certificate) const;

private:
    ::mbedtls_x509_crt certificates_;
    std::vector<SHA256Hash> distrustedKeys_;
};

/// Returns the subject of certificate, such as "C=US, O=Example, CN=Example CA".
[[nodiscard]] std::string
getSubjectName(const ::mbedtls_x509_crt& certificate);

} // namespace pomdog::network::detail
