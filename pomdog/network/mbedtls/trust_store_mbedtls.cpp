// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/trust_store_mbedtls.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/mbedtls/mbedtls_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

TrustStoreMbedTLS::TrustStoreMbedTLS() noexcept
{
    mbedtls_x509_crt_init(&certificates_);
}

TrustStoreMbedTLS::~TrustStoreMbedTLS()
{
    mbedtls_x509_crt_free(&certificates_);
}

std::expected<void, std::unique_ptr<Error>>
TrustStoreMbedTLS::initialize(std::span<const u8> pem)
{
    // NOTE: Certificate parsing uses PSA crypto.
    if (auto result = initializePSACrypto(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    // NOTE: Mbed TLS reads PEM as a null-terminated string, and counts the
    // null character in the size.
    std::vector<u8> text;
    text.reserve(pem.size() + 1);
    text.assign(std::begin(pem), std::end(pem));
    text.push_back(0);

    const auto ret = mbedtls_x509_crt_parse(&certificates_, text.data(), text.size());
    if (ret < 0) {
        return std::unexpected(errors::make("mbedtls_x509_crt_parse failed, " + mbedTLSErrorToString(ret)));
    }
    if (ret > 0) {
        // NOTE: Mbed TLS returns the number of certificates that it could not parse.
        return std::unexpected(errors::make("failed to parse " + std::to_string(ret) + " certificates in the PEM"));
    }
    return {};
}

::mbedtls_x509_crt*
TrustStoreMbedTLS::getCertificates() noexcept
{
    return &certificates_;
}

} // namespace pomdog::network::detail
