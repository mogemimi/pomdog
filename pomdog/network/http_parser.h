// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <span>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

enum class HTTPParseResult {
    WouldBlock,
    Error,
    EndOfFile,
};

class HTTPParser final {
public:
    std::tuple<HTTPParseResult, std::unique_ptr<Error>>
    Parse(std::span<std::uint8_t> view);

    std::unique_ptr<HTTPResponse> GetResponse();

private:
    std::vector<char> rawData;
    std::unique_ptr<HTTPResponse> response;
    std::size_t requiredContentLength = 0;
};

} // namespace pomdog::detail
