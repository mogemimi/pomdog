// Copyright mogemimi. Distributed under the MIT license.

#include "executor.h"
#include "tests/testing/testing.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_response.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;

// NOTE: httpbin.org echoes each request as JSON. Its formatting, header names,
// and header order change over time, so the tests look headers up by name and
// JSON members by key and value instead of comparing whole texts.

namespace {

std::string
toLower(std::string_view in)
{
    const auto asciiToLower = [](char c) -> char {
        if (c <= 'Z' && c >= 'A') {
            return c - ('Z' - 'z');
        }
        return c;
    };

    std::string s{in};
    std::transform(std::begin(s), std::end(s), std::begin(s), asciiToLower);
    return s;
}

/// Returns the value of the header named name, compared without case, or
/// nullopt when the response has none.
[[nodiscard]] std::optional<std::string>
findHeader(const HTTPResponse& response, std::string_view name)
{
    const auto lowerName = toLower(name);
    for (const auto& [key, value] : response.Header) {
        if (toLower(key) == lowerName) {
            return value;
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::string_view
trimLeadingSpaces(std::string_view text) noexcept
{
    while (!text.empty() && ((text.front() == ' ') || (text.front() == '\n') || (text.front() == '\r') || (text.front() == '\t'))) {
        text.remove_prefix(1);
    }
    return text;
}

/// Returns true when json has a member named key whose value is the JSON text
/// value, such as `"hello"` or `42`, at any depth. Spaces around the colon do
/// not matter.
[[nodiscard]] bool
hasMember(std::string_view json, std::string_view key, std::string_view value)
{
    const auto quotedKey = "\"" + std::string{key} + "\"";
    for (auto position = json.find(quotedKey); position != std::string_view::npos; position = json.find(quotedKey, position + 1)) {
        auto rest = trimLeadingSpaces(json.substr(position + quotedKey.size()));
        if (!rest.starts_with(':')) {
            continue;
        }
        rest = trimLeadingSpaces(rest.substr(1));
        if (!rest.starts_with(value)) {
            continue;
        }
        const auto after = trimLeadingSpaces(rest.substr(value.size()));
        if (after.empty() || after.starts_with(',') || after.starts_with('}')) {
            return true;
        }
    }
    return false;
}

/// Checks the status line and the headers of an httpbin.org response, and
/// returns its JSON body, or nullopt when the server did not answer with 200.
[[nodiscard]] std::optional<std::string>
checkJSONResponse(const HTTPResponse& response)
{
    REQUIRE((response.Protocol == "HTTP/1.1" || response.Protocol == "HTTP/1.0"));

    // NOTE: Accept any valid HTTP status code (server may return errors like 502)
    REQUIRE(response.StatusCode >= 100);
    REQUIRE(response.StatusCode < 600);
    REQUIRE(!response.Status.empty());
    REQUIRE(!response.Header.empty());
    if (response.StatusCode != 200) {
        MESSAGE("the server answered " << response.StatusCode << " " << response.Status);
        return std::nullopt;
    }

    REQUIRE(findHeader(response, "Date").has_value());
    REQUIRE(findHeader(response, "Content-Type") == "application/json");
    REQUIRE(findHeader(response, "Content-Length") == std::to_string(response.ContentLength));
    REQUIRE(response.ContentLength == response.Body.size());
    return std::string{reinterpret_cast<const char*>(response.Body.data()), response.Body.size()};
}

void setBody(HTTPRequest& request, std::string_view text)
{
    request.Body.resize(text.size());
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    std::memcpy(request.Body.data(), text.data(), text.size());
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

} // namespace

TEST_CASE("Transfer-Encoding has a 'chunked' operation.")
{
    Executor executor;

    auto request = HTTPRequest::Create(HTTPMethod::Get, "https://www.google.com");
    request->AddHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X x.y; rv:42.0) Gecko/20100101 Firefox/42.0");
    auto conn = request->OnCompleted.connect([&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        // NOTE: Verify HTTP client correctly parses the response protocol
        REQUIRE((response->Protocol == "HTTP/1.1" || response->Protocol == "HTTP/1.0"));

        // NOTE: Accept any valid HTTP status code (server may return errors like 502)
        REQUIRE(response->StatusCode >= 100);
        REQUIRE(response->StatusCode < 600);
        REQUIRE(!response->Status.empty());

        // NOTE: For successful responses, verify body is present
        if (response->StatusCode == 200) {
            REQUIRE(!response->Body.empty());
        }

        executor.ExitLoop();
    });

    auto client = HTTPClient{executor.GetService()};
    if (auto err = client.Do(request); err != nullptr) {
        reportNetworkError(*err);
        return;
    }

    executor.RunLoop();
}

TEST_CASE("HTTPClient Get")
{
    Executor executor;

    auto request = HTTPRequest::Create(HTTPMethod::Get, "http://httpbin.org/get");
    auto conn = request->OnCompleted.connect([&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        if (const auto json = checkJSONResponse(*response); json) {
            REQUIRE(hasMember(*json, "args", "{}"));
            REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
            REQUIRE(hasMember(*json, "url", "\"http://httpbin.org/get\""));
        }

        executor.ExitLoop();
    });

    auto client = HTTPClient{executor.GetService()};
    if (auto err = client.Do(request); err != nullptr) {
        reportNetworkError(*err);
        return;
    }

    executor.RunLoop();
}

TEST_CASE("HTTPClient Post")
{
    Executor executor;

    auto request = HTTPRequest::Create(HTTPMethod::Post, "http://httpbin.org/post");
    auto conn = request->OnCompleted.connect([&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        if (const auto json = checkJSONResponse(*response); json) {
            REQUIRE(hasMember(*json, "data", "\"hello, hi\""));
            REQUIRE(hasMember(*json, "Content-Length", "\"9\""));
            REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
            REQUIRE(hasMember(*json, "json", "null"));
            REQUIRE(hasMember(*json, "url", "\"http://httpbin.org/post\""));
        }

        executor.ExitLoop();
    });

    setBody(*request, "hello, hi");

    auto client = HTTPClient{executor.GetService()};
    if (auto err = client.Do(request); err != nullptr) {
        reportNetworkError(*err);
        return;
    }

    executor.RunLoop();
}

TEST_CASE("HTTPClient Get Secure")
{
    Executor executor;

    auto request = HTTPRequest::Create(HTTPMethod::Get, "https://httpbin.org/get");
    auto conn = request->OnCompleted.connect([&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        if (const auto json = checkJSONResponse(*response); json) {
            REQUIRE(hasMember(*json, "args", "{}"));
            REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
            REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/get\""));
        }

        executor.ExitLoop();
    });

    auto client = HTTPClient{executor.GetService()};
    if (auto err = client.Do(request); err != nullptr) {
        reportNetworkError(*err);
        return;
    }

    executor.RunLoop();
}

TEST_CASE("HTTPClient Post Secure")
{
    Executor executor;

    auto request = HTTPRequest::Create(HTTPMethod::Post, "https://httpbin.org/post");
    setBody(*request, "hello, hi");

    auto conn = request->OnCompleted.connect([&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        if (const auto json = checkJSONResponse(*response); json) {
            REQUIRE(hasMember(*json, "data", "\"hello, hi\""));
            REQUIRE(hasMember(*json, "Content-Length", "\"9\""));
            REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
            REQUIRE(hasMember(*json, "json", "null"));
            REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/post\""));
        }

        executor.ExitLoop();
    });

    auto client = HTTPClient{executor.GetService()};
    if (auto err = client.Do(request); err != nullptr) {
        reportNetworkError(*err);
        return;
    }

    executor.RunLoop();
}

TEST_CASE("HTTPClient::Get")
{
    Executor executor;

    auto callback = [&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        if (const auto json = checkJSONResponse(*response); json) {
            REQUIRE(hasMember(*json, "args", "{}"));
            REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/get\""));
        }

        executor.ExitLoop();
    };

    auto client = HTTPClient{executor.GetService()};
    auto [conn, err] = client.Get("https://httpbin.org/get", std::move(callback));
    REQUIRE(err == nullptr);
    REQUIRE(conn.isConnected());

    executor.RunLoop();
}

TEST_CASE("HTTPClient::Post")
{
    Executor executor;

    auto callback = [&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            reportNetworkError(*err);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response != nullptr);

        if (const auto json = checkJSONResponse(*response); json) {
            REQUIRE(hasMember(*json, "Content-Length", "\"30\""));
            REQUIRE(hasMember(*json, "Content-Type", "\"application/json\""));
            REQUIRE(hasMember(*json, "answer", "42"));
            REQUIRE(hasMember(*json, "text", "\"hello\""));
            REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/post\""));
        }

        executor.ExitLoop();
    };

    std::string text = R"({"answer":42, "text": "hello"})";
    std::vector<char> buffer;
    buffer.resize(text.size());
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    std::memcpy(buffer.data(), text.data(), buffer.size());
    POMDOG_CLANG_UNSAFE_BUFFER_END

    auto client = HTTPClient{executor.GetService()};
    auto [conn, err] = client.Post("https://httpbin.org/post", "application/json", std::move(buffer), std::move(callback));
    REQUIRE(err == nullptr);
    REQUIRE(conn.isConnected());

    executor.RunLoop();
}

TEST_CASE("multiple connection")
{
    Executor executor;

    auto client = HTTPClient{executor.GetService()};

    auto callback1 = [&](std::unique_ptr<HTTPResponse>&& response1, const std::unique_ptr<Error>& err1) {
        if (err1 != nullptr) {
            reportNetworkError(*err1);
            executor.ExitLoop();
            return;
        }
        REQUIRE(response1 != nullptr);

        // NOTE: Verify HTTP client correctly parses the response protocol
        REQUIRE((response1->Protocol == "HTTP/1.1" || response1->Protocol == "HTTP/1.0"));

        // NOTE: Accept any valid HTTP status code (server may return errors like 502)
        REQUIRE(response1->StatusCode >= 100);
        REQUIRE(response1->StatusCode < 600);
        REQUIRE(!response1->Status.empty());

        auto callback2 = [&](std::unique_ptr<HTTPResponse>&& response2, const std::unique_ptr<Error>& err2) {
            if (err2 != nullptr) {
                reportNetworkError(*err2);
                executor.ExitLoop();
                return;
            }
            REQUIRE(response2 != nullptr);

            // NOTE: Verify HTTP client correctly parses the response protocol
            REQUIRE((response2->Protocol == "HTTP/1.1" || response2->Protocol == "HTTP/1.0"));

            // NOTE: Accept any valid HTTP status code (server may return errors like 502)
            REQUIRE(response2->StatusCode >= 100);
            REQUIRE(response2->StatusCode < 600);
            REQUIRE(!response2->Status.empty());
            executor.ExitLoop();
        };

        auto [conn2, err2] = client.Get("https://httpbin.org/get", std::move(callback2));
        REQUIRE(err2 == nullptr);
        REQUIRE(conn2.isConnected());
    };

    auto [conn, err] = client.Get("https://httpbin.org/get", std::move(callback1));
    REQUIRE(err == nullptr);
    REQUIRE(conn.isConnected());

    executor.RunLoop();
}

TEST_CASE("Cancel HTTP Request")
{
    Executor executor;

    auto request = HTTPRequest::Create(HTTPMethod::Get, "http://httpbin.org/get");
    auto conn = request->OnCompleted.connect([&](std::unique_ptr<HTTPResponse>&& response, const std::unique_ptr<Error>& err) {
        REQUIRE(err != nullptr);
        REQUIRE(response == nullptr);
        executor.ExitLoop();
    });

    setBody(*request, "hello, hi");

    auto client = HTTPClient{executor.GetService()};
    if (auto err = client.Do(request); err != nullptr) {
        reportNetworkError(*err);
        return;
    }

    auto err = client.CancelRequest(request);
    REQUIRE(err == nullptr);

    executor.RunLoop();
}
