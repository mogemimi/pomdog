// Copyright mogemimi. Distributed under the MIT license.

#include "executor.h"
#include "tests/testing/testing.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/signals/connection_list.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cstring>
#include <sstream>
#include <thread>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;

TEST_CASE("TCP connection for HTTP client")
{
    Executor executor;
    ConnectionList conn;

    const auto hostName = "google.com";
    const auto path = "/";
    const auto port = "80";

    std::unordered_map<std::string, std::string> fields;
    fields.emplace("Host", hostName);
    fields.emplace("Connection", "close");

    auto [streamResult, connectErr] = TCPStream::connect(executor.GetService(), std::string{hostName} + ":" + port, std::chrono::seconds(5));
    if (connectErr != nullptr) {
        MESSAGE(connectErr->toString());
        return;
    }

    // NOTE: According to C++ Standard Core issue 2313, structured bindings are
    // never capturable because they are never names of variables. The following
    // code is a workaround that makes structured bindings captuable.
    auto stream = std::move(streamResult);

    stream.setTimeout(std::chrono::seconds{5});

    conn += stream.onConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            INFO("unable to connect server:", err->toString());
            executor.ExitLoop();
            return;
        }

        REQUIRE(stream.isConnected());

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

        auto writeErr = stream.write(std::span<const std::uint8_t>{
            reinterpret_cast<const std::uint8_t*>(header.data()),
            header.size()});
        REQUIRE(writeErr == nullptr);
    });
    conn += stream.onDisconnect([&] {
        REQUIRE_FALSE(stream.isConnected());
    });
    conn += stream.onRead([&](std::span<std::uint8_t> view, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            INFO("unable to connect server:", err->toString());
            stream.disconnect();
            executor.ExitLoop();
            return;
        }

        std::string_view text(reinterpret_cast<const char*>(view.data()), view.size());

        constexpr auto html = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.google.com/";
        REQUIRE(strings::hasPrefix(text, html));

        REQUIRE(stream.isConnected());
        stream.disconnect();
        REQUIRE_FALSE(stream.isConnected());

        executor.ExitLoop();
    });

    executor.RunLoop();

    REQUIRE_FALSE(stream.isConnected());
}
