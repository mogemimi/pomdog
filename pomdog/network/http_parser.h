// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_response.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

enum class HTTPParseResult : u8 {
    WouldBlock,
    EndOfFile,
};

class HTTPParser final {
public:
    HTTPParser() = default;

    HTTPParser(const HTTPParser&) = delete;
    HTTPParser& operator=(const HTTPParser&) = delete;
    HTTPParser(HTTPParser&&) = delete;
    HTTPParser& operator=(HTTPParser&&) = delete;

    /// Reads the next bytes of a response. Returns EndOfFile once the response
    /// is complete, and WouldBlock while it needs more bytes.
    [[nodiscard]] std::expected<HTTPParseResult, std::unique_ptr<Error>>
    parse(std::span<const u8> data);

    /// Takes the response after `parse()` returned EndOfFile.
    [[nodiscard]] HTTPResponse
    takeResponse();

private:
    std::vector<u8> rawData_;
    std::optional<HTTPResponse> response_;
    std::size_t requiredContentLength_ = 0;
};

} // namespace pomdog::network::detail
