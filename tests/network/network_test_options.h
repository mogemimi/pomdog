// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/testing/testing.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: tools/cmd/network-test-runner starts the local servers and passes their
// addresses on the command line. Its README describes how each server answers.
// The TLS servers have certificates for the IP address 127.0.0.1, and all of
// them except the untrusted one chain up to the root in `getCAFile()`.

namespace pomdog::testing {

/// Returns the address of the local HTTP server, such as `127.0.0.1:50000`,
/// that `--http-server` named.
[[nodiscard]] std::string_view
getHTTPServerAddress() noexcept;

/// Returns the address of the local HTTPS server, which answers as the HTTP
/// server does and `--https-server` named.
[[nodiscard]] std::string_view
getHTTPSServerAddress() noexcept;

/// Returns the address of the local TCP server that echoes each line, which
/// `--tcp-server` named.
[[nodiscard]] std::string_view
getTCPServerAddress() noexcept;

/// Returns the address of the local TLS server that echoes each line, which
/// `--tls-server` named.
[[nodiscard]] std::string_view
getTLSServerAddress() noexcept;

/// Returns the address of the local TLS server that echoes each line with a
/// self-signed certificate, which `--untrusted-tls-server` named.
[[nodiscard]] std::string_view
getUntrustedTLSServerAddress() noexcept;

/// Returns the address of the local UDP server that echoes each datagram,
/// which `--udp-server` named.
[[nodiscard]] std::string_view
getUDPServerAddress() noexcept;

/// Returns an address where nothing listens, so that a TCP connection to it is
/// refused, which `--closed-tcp-server` named.
[[nodiscard]] std::string_view
getClosedTCPServerAddress() noexcept;

/// Returns the path of the root certificate in PEM format, which `--ca-file`
/// named.
[[nodiscard]] std::string_view
getCAFile() noexcept;

/// Returns the SHA-256 hash of the SubjectPublicKeyInfo of the root in
/// `--ca-file`, in hexadecimal, which `--ca-key-hash` named.
[[nodiscard]] std::string_view
getCAKeyHash() noexcept;

/// Returns the SHA-256 hash of the SubjectPublicKeyInfo of the certificate of
/// the local HTTPS and TLS servers, in hexadecimal, which `--server-key-hash`
/// named.
[[nodiscard]] std::string_view
getServerKeyHash() noexcept;

} // namespace pomdog::testing
