// Copyright mogemimi. Distributed under the MIT license.

#include "executor.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/signals/connection_list.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"
#include <catch_amalgamated.hpp>
#include <cstring>
#include <sstream>
#include <thread>
#include <unordered_map>

using namespace pomdog;

TEST_CASE("TCP connection for HTTP client", "[Network]")
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
        WARN(connectErr->toString());
        return;
    }

    // NOTE: According to C++ Standard Core issue 2313, structured bindings are
    // never capturable because they are never names of variables. The following
    // code is a workaround that makes structured bindings captuable.
    auto stream = std::move(streamResult);

    stream.setTimeout(std::chrono::seconds{5});

    conn += stream.onConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to connect server");
            executor.ExitLoop();
            return;
        }

        REQUIRE(stream.isConnected());

        // NOTE: Write the GET request
        std::ostringstream ss;
        ss << "GET " << path;
        ss << " HTTP/1.1" << "\n";
        for (const auto& field : fields) {
            ss << field.first << ": " << field.second << "\n";
        }
        ss << "\n";

        std::string header = ss.str();

        auto writeErr = stream.write(ArrayView<char const>{header.data(), header.size()}.viewAs<std::uint8_t const>());
        REQUIRE(writeErr == nullptr);
    });
    conn += stream.onDisconnect([&] {
        REQUIRE_FALSE(stream.isConnected());
    });
    conn += stream.onRead([&](const ArrayView<std::uint8_t>& view, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to connect server");
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
