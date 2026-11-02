// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
#include "tests/testing/testing.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;

TEST_CASE("TLS connection for HTTPS client")
{
    const auto hostName = "www.google.com";
    const auto path = "/";
    const auto port = "443";

    std::unordered_map<std::string, std::string> fields;
    fields.emplace("Host", hostName);
    fields.emplace("Connection", "close");

    StreamOptions options;
    options.idleTimeout = std::chrono::seconds{5};

    auto result = connectTLSStream(std::string{hostName} + ":" + port, options);
    if (!result) {
        reportNetworkError(*result.error());
        return;
    }
    auto stream = std::move(*result);

    const auto finished = runUntil([&] {
        stream->update();
        while (auto event = stream->pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                REQUIRE(stream->isConnected());

                // NOTE: Write the GET request
                std::ostringstream ss;
                ss << "GET " << path;
                ss << " HTTP/1.1"
                   << "\n";
                for (const auto& field : fields) {
                    ss << field.first << ": " << field.second << "\n";
                }
                ss << "\n";

                std::string header = ss.str();

                POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
                auto buf = std::span<const u8>{
                    reinterpret_cast<const u8*>(header.data()),
                    header.size()};
                POMDOG_CLANG_UNSAFE_BUFFER_END
                REQUIRE(stream->write(buf).has_value());
            }
            else if (event->kind == StreamEventKind::Received) {
                std::string_view text(reinterpret_cast<const char*>(event->data.data()), event->data.size());

                constexpr auto html = "HTTP/1.1 200 OK\r\n";
                REQUIRE(strings::hasPrefix(text, html));

                REQUIRE(stream->isConnected());
                stream->close();
                REQUIRE_FALSE(stream->isConnected());
                return true;
            }
            else {
                REQUIRE(event->kind == StreamEventKind::Disconnected);
                REQUIRE_FALSE(stream->isConnected());
                REQUIRE(event->error != nullptr);
                reportNetworkError(*event->error);
                return true;
            }
        }
        return false;
    });
    REQUIRE(finished);

    REQUIRE_FALSE(stream->isConnected());
    REQUIRE_FALSE(stream->pollEvent().has_value());
}

TEST_CASE("TLS stream that is closed while connecting reports no events")
{
    auto result = connectTLSStream("www.google.com:443", StreamOptions{});
    if (!result) {
        reportNetworkError(*result.error());
        return;
    }
    auto stream = std::move(*result);

    stream->close();
    REQUIRE_FALSE(stream->isConnected());

    for (int i = 0; i < 10; ++i) {
        stream->update();
        REQUIRE_FALSE(stream->pollEvent().has_value());
    }
    REQUIRE_FALSE(stream->write(std::span<const u8>{}).has_value());
}
