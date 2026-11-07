// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/mbedtls_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#include "mbedtls/error.h"
#include "psa/crypto.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

// NOTE: PSA_SUCCESS expands to a C-style cast, which -Wold-style-cast rejects.
POMDOG_CLANG_SUPPRESS_WARNING_PUSH
POMDOG_CLANG_SUPPRESS_WARNING("-Wold-style-cast")
POMDOG_GCC_SUPPRESS_WARNING_PUSH
POMDOG_GCC_SUPPRESS_WARNING("-Wold-style-cast")
constexpr psa_status_t psaSuccess = PSA_SUCCESS;
constexpr psa_algorithm_t psaSHA256 = PSA_ALG_SHA_256;
POMDOG_GCC_SUPPRESS_WARNING_POP
POMDOG_CLANG_SUPPRESS_WARNING_POP

} // namespace

std::string mbedTLSErrorToString(int err)
{
    std::array<char, 128> buf;
    std::fill(std::begin(buf), std::end(buf), '\0');
    mbedtls_strerror(err, buf.data(), buf.size());
    buf.back() = 0;
    return buf.data();
}

std::expected<void, std::unique_ptr<Error>>
initializePSACrypto()
{
    if (auto status = psa_crypto_init(); status != psaSuccess) {
        return std::unexpected(errors::make("psa_crypto_init failed, " + mbedTLSErrorToString(status)));
    }
    return {};
}

std::expected<SHA256Hash, std::unique_ptr<Error>>
computeSHA256(std::span<const u8> data)
{
    SHA256Hash hash = {};
    std::size_t hashSize = 0;
    if (auto status = psa_hash_compute(psaSHA256, data.data(), data.size(), hash.data(), hash.size(), &hashSize); status != psaSuccess) {
        return std::unexpected(errors::make("psa_hash_compute failed, " + mbedTLSErrorToString(status)));
    }
    if (hashSize != hash.size()) {
        return std::unexpected(errors::make("psa_hash_compute returned " + std::to_string(hashSize) + " bytes for SHA-256"));
    }
    return hash;
}

} // namespace pomdog::network::detail
