// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/trust_store_mbedtls.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/network/mbedtls/mbedtls_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/distrusted_keys_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// Reads the sorted key hashes of a `.dkeys` file.
[[nodiscard]] std::expected<std::vector<SHA256Hash>, std::unique_ptr<Error>>
readDistrustedKeys(std::span<const u8> data)
{
    auto verifier = flatbuffers::Verifier(data.data(), data.size());
    if (!pomdogschemas::VerifyDistrustedKeysBuffer(verifier)) {
        return std::unexpected(errors::make("the distrusted keys are not a valid .dkeys file"));
    }

    const auto* keyHashes = pomdogschemas::GetDistrustedKeys(data.data())->key_hashes();
    if (keyHashes == nullptr) {
        return std::vector<SHA256Hash>{};
    }

    const auto bytes = std::span<const u8>{keyHashes->data(), keyHashes->size()};
    if ((bytes.size() % std::tuple_size_v<SHA256Hash>) != 0) {
        return std::unexpected(errors::make("the distrusted keys hold " + std::to_string(bytes.size()) + " bytes, which is not a multiple of 32"));
    }

    std::vector<SHA256Hash> hashes(bytes.size() / std::tuple_size_v<SHA256Hash>);
    for (std::size_t i = 0; i < hashes.size(); ++i) {
        const auto hash = bytes.subspan(i * std::tuple_size_v<SHA256Hash>, std::tuple_size_v<SHA256Hash>);
        std::ranges::copy(hash, std::begin(hashes[i]));
    }

    // NOTE: The lookup searches by halves, so the file must keep its order.
    if (std::ranges::adjacent_find(hashes, std::ranges::greater_equal{}) != std::end(hashes)) {
        return std::unexpected(errors::make("the distrusted keys are not sorted without duplicates"));
    }
    return hashes;
}

} // namespace

TrustStoreMbedTLS::TrustStoreMbedTLS() noexcept
{
    mbedtls_x509_crt_init(&certificates_);
}

TrustStoreMbedTLS::~TrustStoreMbedTLS()
{
    mbedtls_x509_crt_free(&certificates_);
}

std::expected<void, std::unique_ptr<Error>>
TrustStoreMbedTLS::initialize(const TrustStoreDesc& desc)
{
    // NOTE: Certificate parsing and hashing use PSA crypto.
    if (auto result = initializePSACrypto(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    // NOTE: Mbed TLS reads PEM as a null-terminated string, and counts the
    // null character in the size.
    std::vector<u8> text;
    text.reserve(desc.rootCertificatesPEM.size() + 1);
    text.assign(std::begin(desc.rootCertificatesPEM), std::end(desc.rootCertificatesPEM));
    text.push_back(0);

    const auto ret = mbedtls_x509_crt_parse(&certificates_, text.data(), text.size());
    if (ret < 0) {
        return std::unexpected(errors::make("mbedtls_x509_crt_parse failed, " + mbedTLSErrorToString(ret)));
    }
    if (ret > 0) {
        // NOTE: Mbed TLS returns the number of certificates that it could not parse.
        return std::unexpected(errors::make("failed to parse " + std::to_string(ret) + " certificates in the PEM"));
    }

    if (!desc.distrustedKeys.empty()) {
        auto keys = readDistrustedKeys(desc.distrustedKeys);
        if (!keys) {
            return std::unexpected(std::move(keys.error()));
        }
        distrustedKeys_ = std::move(*keys);
    }

    // NOTE: A root that is also distrusted is a mistake in the shipped data.
    for (const auto* root = &certificates_; root != nullptr; root = root->next) {
        if (isDistrusted(*root)) {
            return std::unexpected(errors::make("the root certificate " + getSubjectName(*root) + " has a distrusted key"));
        }
    }
    return {};
}

::mbedtls_x509_crt*
TrustStoreMbedTLS::getCertificates() noexcept
{
    return &certificates_;
}

bool TrustStoreMbedTLS::hasDistrustedKeys() const noexcept
{
    return !distrustedKeys_.empty();
}

bool TrustStoreMbedTLS::isDistrusted(const ::mbedtls_x509_crt& certificate) const
{
    if (distrustedKeys_.empty()) {
        return false;
    }

    // NOTE: pk_raw is the SubjectPublicKeyInfo as the certificate holds it,
    // which distrusted-keys-gen hashes too.
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto key = std::span<const u8>{certificate.pk_raw.p, certificate.pk_raw.len};
    POMDOG_CLANG_UNSAFE_BUFFER_END

    auto hash = computeSHA256(key);
    if (!hash) {
        // NOTE: Fail closed, since the key cannot be checked.
        return true;
    }
    return std::ranges::binary_search(distrustedKeys_, *hash);
}

std::string
getSubjectName(const ::mbedtls_x509_crt& certificate)
{
    std::array<char, 512> buffer = {};
    if (mbedtls_x509_dn_gets(buffer.data(), buffer.size(), &certificate.subject) < 0) {
        return "(unknown subject)";
    }
    buffer.back() = 0;
    return buffer.data();
}

} // namespace pomdog::network::detail
