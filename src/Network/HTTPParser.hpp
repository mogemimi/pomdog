// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

namespace Pomdog::Detail {

enum class HTTPParseResult {
    WouldBlock,
    Error,
    EndOfFile,
};

class HTTPParser final {
public:
    std::tuple<HTTPParseResult, std::shared_ptr<Error>>
    Parse(const ArrayView<std::uint8_t>& view);

    std::unique_ptr<HTTPResponse> GetResponse();

private:
    std::vector<char> rawData;
    std::unique_ptr<HTTPResponse> response;
    std::size_t requiredContentLength = 0;
};

} // namespace Pomdog::Detail
