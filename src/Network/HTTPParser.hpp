// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

enum class HTTPParseResult {
    WouldBlock,
    Error,
    EndOfFile,
};

class HTTPParser final {
public:
    std::tuple<HTTPParseResult, std::unique_ptr<Error>>
    Parse(const ArrayView<std::uint8_t>& view);

    std::unique_ptr<HTTPResponse> GetResponse();

private:
    std::vector<char> rawData;
    std::unique_ptr<HTTPResponse> response;
    std::size_t requiredContentLength = 0;
};

} // namespace Pomdog::Detail
