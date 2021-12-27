// Copyright mogemimi. Distributed under the MIT license.

#include "executor.h"
#include "pomdog/chrono/game_clock.hpp"
#include "pomdog/network/array_view.hpp"
#include "pomdog/network/io_service.hpp"
#include "pomdog/network/tcp_stream.hpp"
#include "pomdog/signals/connection_list.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/string_helper.hpp"
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

    auto [streamResult, err] = TCPStream::Connect(executor.GetService(), std::string{hostName} + ":" + port, std::chrono::seconds(5));
    if (err != nullptr) {
        WARN(err->ToString());
        return;
    }

    // NOTE: According to C++ Standard Core issue 2313, structured bindings are
    // never capturable because they are never names of variables. The following
    // code is a workaround that makes structured bindings captuable.
    auto stream = std::move(streamResult);

    stream.SetTimeout(std::chrono::seconds{5});

    conn += stream.OnConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to connect server");
            executor.ExitLoop();
            return;
        }

        REQUIRE(stream.IsConnected());

        // NOTE: Write the GET request
        std::ostringstream ss;
        ss << "GET " << path;
        ss << " HTTP/1.1" << "\n";
        for (const auto& field : fields) {
            ss << field.first << ": " << field.second << "\n";
        }
        ss << "\n";

        std::string header = ss.str();

        auto writeErr = stream.Write(ArrayView<char const>{header.data(), header.size()}.ViewAs<std::uint8_t const>());
        REQUIRE(writeErr == nullptr);
    });
    conn += stream.OnDisconnect([&] {
        REQUIRE_FALSE(stream.IsConnected());
    });
    conn += stream.OnRead([&](const ArrayView<std::uint8_t>& view, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to connect server");
            stream.Disconnect();
            executor.ExitLoop();
            return;
        }

        std::string_view text(reinterpret_cast<const char*>(view.GetData()), view.GetSize());

        constexpr auto html = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.google.com/";
        REQUIRE(StringHelper::HasPrefix(text, html));

        REQUIRE(stream.IsConnected());
        stream.Disconnect();
        REQUIRE_FALSE(stream.IsConnected());

        executor.ExitLoop();
    });

    executor.RunLoop();

    REQUIRE_FALSE(stream.IsConnected());
}
