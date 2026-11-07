// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_response.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Limits the status line and header fields of a response, and the trailer
/// fields of a chunked body, so that a server cannot send them without end.
inline constexpr std::size_t maxResponseHeaderSize = 64 * 1024;

/// Parses the status line and the header fields of a response into response.
/// header must end with the empty line that ends the header fields.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
parseResponseHeader(HTTPResponse& response, std::string_view header);

/// Identifies what `HTTPParser` found in the bytes so far.
enum class HTTPParseResult : u8 {
    /// The response needs more bytes.
    WouldBlock,

    /// The response is complete.
    EndOfFile,
};

/// HTTPParser reads an HTTP/1.x response from the bytes of a connection.
///
/// A body has the length of its Content-Length header, is chunked, or lasts
/// until the server closes the connection. Chunked encoding wins over
/// Content-Length. Responses to HEAD requests, and those with status 204 or
/// 304, have no body. Informational responses (1xx) before the final one are
/// skipped.
class HTTPParser final {
public:
    /// Creates a parser for the response to a request with method. A body
    /// longer than maxBodySize bytes fails the response. Zero sets no limit.
    HTTPParser(HTTPMethod method, std::size_t maxBodySize) noexcept;

    HTTPParser(const HTTPParser&) = delete;
    HTTPParser& operator=(const HTTPParser&) = delete;
    HTTPParser(HTTPParser&&) = delete;
    HTTPParser& operator=(HTTPParser&&) = delete;

    /// Reads the next bytes of the response. Returns EndOfFile once the
    /// response is complete, and WouldBlock while it needs more bytes. Bytes
    /// after a complete response are ignored.
    [[nodiscard]] std::expected<HTTPParseResult, std::unique_ptr<Error>>
    parse(std::span<const u8> data);

    /// Tells the parser that the server closed the connection. Returns
    /// EndOfFile when that ends the response, as for a body without a length,
    /// and an error when the response is incomplete.
    [[nodiscard]] std::expected<HTTPParseResult, std::unique_ptr<Error>>
    finish();

    /// Takes the response after `parse()` or `finish()` returned EndOfFile.
    [[nodiscard]] HTTPResponse
    takeResponse();

private:
    enum class State : u8 {
        Header,
        FixedBody,
        ChunkSize,
        ChunkData,
        ChunkDataEnd,
        Trailer,
        BodyUntilClose,
        Complete,
    };

    /// Reads as much as the unread bytes allow in the current state. Returns
    /// false when it needs more bytes.
    [[nodiscard]] std::expected<bool, std::unique_ptr<Error>>
    advance();

    [[nodiscard]] std::expected<bool, std::unique_ptr<Error>>
    readHeader();

    [[nodiscard]] std::expected<bool, std::unique_ptr<Error>>
    readChunkSize();

    [[nodiscard]] std::expected<bool, std::unique_ptr<Error>>
    readChunkDataEnd();

    [[nodiscard]] std::expected<bool, std::unique_ptr<Error>>
    readTrailer();

    /// Moves up to remaining_ unread bytes into the body, or all of them when
    /// the body lasts until the connection closes.
    [[nodiscard]] std::expected<bool, std::unique_ptr<Error>>
    readBody();

    [[nodiscard]] std::span<const u8>
    unread() const noexcept;

private:
    /// The received bytes. Those before offset_ are read.
    std::vector<u8> buffer_;
    std::size_t offset_ = 0;

    /// How many unread bytes the search for the end of the header has seen.
    std::size_t headerScanned_ = 0;

    /// The bytes of the trailer fields read so far.
    std::size_t trailerSize_ = 0;

    /// The bytes left of a fixed body or of the current chunk.
    std::size_t remaining_ = 0;

    std::size_t maxBodySize_ = 0;
    std::optional<HTTPResponse> response_;
    HTTPMethod method_ = HTTPMethod::Get;
    State state_ = State::Header;
};

} // namespace pomdog::network::detail
