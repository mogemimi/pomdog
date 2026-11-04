// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>

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

/// TrustStoreMbedTLS holds root certificates as an Mbed TLS certificate chain.
class TrustStoreMbedTLS final : public TrustStore {
public:
    TrustStoreMbedTLS() noexcept;

    ~TrustStoreMbedTLS() override;

    /// Parses the certificates in pem. Fails unless every certificate parses.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    initialize(std::span<const u8> pem);

    /// Returns the certificate chain, which TLS streams verify servers against.
    [[nodiscard]] ::mbedtls_x509_crt*
    getCertificates() noexcept;

private:
    ::mbedtls_x509_crt certificates_;
};

} // namespace pomdog::network::detail
