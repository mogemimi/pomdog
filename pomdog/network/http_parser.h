// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

enum class HTTPParseResult : u8 {
    WouldBlock,
    Error,
    EndOfFile,
};

class HTTPParser final {
public:
    HTTPParser() = default;

    HTTPParser(const HTTPParser&) = delete;
    HTTPParser& operator=(const HTTPParser&) = delete;
    HTTPParser(HTTPParser&&) = delete;
    HTTPParser& operator=(HTTPParser&&) = delete;

    std::tuple<HTTPParseResult, std::unique_ptr<Error>>
    parse(std::span<u8> view);

    std::unique_ptr<HTTPResponse> getResponse();

private:
    std::vector<char> rawData_;
    std::unique_ptr<HTTPResponse> response_;
    std::size_t requiredContentLength_ = 0;
};

} // namespace pomdog::detail
