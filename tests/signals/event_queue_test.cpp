// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/event_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Connection;
using pomdog::EventQueue;

TEST_CASE("EventQueue")
{
    SUBCASE("enqueue")
    {
        EventQueue<int> queue;

        queue.enqueue(42);

        int result = 0;
        auto connection = queue.connect([&result](int x) { result = x; });

        queue.emit();
        REQUIRE(result == 42);
    }
    SUBCASE("enqueue multiple")
    {
        EventQueue<int> queue;
        std::vector<int> results;

        auto connection = queue.connect([&results](int x) { results.push_back(x); });

        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);

        queue.emit();

        REQUIRE(results.size() == 3);
        REQUIRE(results[0] == 1);
        REQUIRE(results[1] == 2);
        REQUIRE(results[2] == 3);
    }
    SUBCASE("clear queue")
    {
        EventQueue<int> queue;
        std::vector<int> results;

        auto connection = queue.connect([&results](int x) { results.push_back(x); });

        queue.enqueue(1);
        queue.enqueue(2);

        queue.emit();

        REQUIRE(results.size() == 2);

        queue.emit();

        REQUIRE(results.size() == 2);
    }
    SUBCASE("multiple listeners")
    {
        EventQueue<int> queue;
        int result1 = 0;
        int result2 = 0;

        auto connection1 = queue.connect([&result1](int x) { result1 = x; });
        auto connection2 = queue.connect([&result2](int x) { result2 = x * 2; });

        queue.enqueue(42);
        queue.emit();

        REQUIRE(result1 == 42);
        REQUIRE(result2 == 84);
    }
    SUBCASE("disconnect")
    {
        EventQueue<int> queue;
        int result = 0;

        auto connection = queue.connect([&result](int x) { result = x; });

        queue.enqueue(42);
        queue.emit();
        REQUIRE(result == 42);

        connection.disconnect();

        queue.enqueue(100);
        queue.emit();
        REQUIRE(result == 42);
    }
    SUBCASE("Invoke with POD struct")
    {
        struct User {
            std::string name;
            int id;
        };

        std::vector<int> integers;
        std::vector<std::string> names;

        EventQueue<std::any> eventQueue;
        auto conn = eventQueue.connect([&](const std::any& event) {
            REQUIRE(std::any_cast<User>(&event) != nullptr);
            auto user = std::any_cast<User>(&event);
            names.push_back(user->name);
            integers.push_back(user->id);
        });
        REQUIRE(conn.isConnected());

        eventQueue.enqueue(std::make_any<User>(User{.name = "Donald", .id = 42}));
        eventQueue.enqueue(std::make_any<User>(User{.name = "Goofy", .id = 43}));
        REQUIRE(names.empty());
        REQUIRE(integers.empty());

        eventQueue.emit();
        REQUIRE(names.size() == 2);
        REQUIRE(names[0] == "Donald");
        REQUIRE(names[1] == "Goofy");
        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SUBCASE("RecursiveConnection")
    {
        std::vector<int> integers;
        std::function<void(int)> slot = [&](int event) {
            integers.push_back(event);
        };

        EventQueue<int> eventQueue;
        auto conn = eventQueue.connect([&](int) {
            auto conn2 = eventQueue.connect(slot);
            REQUIRE(conn2.isConnected());
        });
        REQUIRE(conn.isConnected());

        eventQueue.enqueue(42);
        REQUIRE(integers.empty());
        eventQueue.emit();
        REQUIRE(integers.empty());

        eventQueue.enqueue(43);
        REQUIRE(integers.empty());

        eventQueue.emit();
        REQUIRE(integers.size() == 1);
        REQUIRE(integers[0] == 43);
    }
    SUBCASE("CallingDisconnect")
    {
        std::vector<int> integers;
        std::function<void(int)> slot = [&](int event) {
            integers.push_back(event);
        };

        EventQueue<int> eventQueue;
        Connection conn;
        REQUIRE(!conn.isConnected());
        conn = eventQueue.connect([&](int event) {
            slot(event);
            REQUIRE(conn.isConnected());
            conn.disconnect();
            REQUIRE(!conn.isConnected());
        });
        REQUIRE(conn.isConnected());

        eventQueue.enqueue(42);
        REQUIRE(conn.isConnected());
        REQUIRE(integers.empty());
        eventQueue.emit();
        REQUIRE(!conn.isConnected());
        REQUIRE(integers.size() == 1);
        REQUIRE(integers[0] == 42);

        integers.clear();
        eventQueue.enqueue(43);
        REQUIRE(integers.empty());
        eventQueue.emit();
        REQUIRE(integers.empty());
    }
    SUBCASE("ArgumentPerfectForwarding")
    {
        EventQueue<std::shared_ptr<int>> eventQueue;
        auto conn = eventQueue.connect([&](std::shared_ptr<int> event) {
            REQUIRE((event != nullptr));
        });
        REQUIRE(conn.isConnected());

        {
            auto pointer = std::make_shared<int>(42);
            REQUIRE((pointer != nullptr));
            eventQueue.enqueue(pointer);
            REQUIRE((pointer != nullptr));
        }
        {
            auto pointer = std::make_shared<int>(42);
            REQUIRE((pointer != nullptr));
            eventQueue.enqueue(std::move(pointer));
            REQUIRE((pointer == nullptr));
        }
    }
}
