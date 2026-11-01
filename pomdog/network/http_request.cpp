// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_request.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

HTTPRequest::HTTPRequest() = default;

std::shared_ptr<HTTPRequest>
HTTPRequest::create(HTTPMethod method, const std::string& url)
{
    auto req = std::make_shared<HTTPRequest>();
    req->url = url;
    req->method = method;
    return req;
}

void HTTPRequest::addHeader(const std::string& key, const std::string& value)
{
    this->header.emplace_back(key, value);
}

} // namespace pomdog
