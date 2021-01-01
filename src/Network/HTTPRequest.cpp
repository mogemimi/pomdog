// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Network/HTTPRequest.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

std::shared_ptr<HTTPRequest>
HTTPRequest::Create(HTTPMethod method, const std::string& url)
{
    auto req = std::make_shared<HTTPRequest>();
    req->URL = url;
    req->Method = method;
    return req;
}

void HTTPRequest::AddHeader(const std::string& key, const std::string& value)
{
    this->Headers.emplace_back(key, value);
}

} // namespace Pomdog
