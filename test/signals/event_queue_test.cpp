// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/signals/event_queue.h"
#include <catch_amalgamated.hpp>
#include <any>
#include <utility>

using pomdog::Connection;
using pomdog::EventQueue;

TEST_CASE("EventQueue", "[EventQueue]")
{
    std::vector<int> integers;
    std::vector<std::string> names;
    std::function<void(int)> slot;

    integers.clear();
    names.clear();

    slot = [&](int event) {
        integers.push_back(event);
    };

    SECTION("Invoke with int")
    {
        EventQueue<int> eventQueue;
        auto connection = eventQueue.connect(slot);

        eventQueue.enqueue(42);
        eventQueue.enqueue(43);
        eventQueue.enqueue(44);
        REQUIRE(integers.empty());

        eventQueue.emit();
        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SECTION("Invoke with POD struct")
    {
        struct User {
            std::string name;
            int id;
        };

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
    SECTION("Disconnect")
    {
        EventQueue<int> eventQueue;
        auto connection = eventQueue.connect(slot);

        eventQueue.enqueue(42);
        REQUIRE(integers.empty());

        connection.disconnect();
        eventQueue.emit();
        REQUIRE(integers.empty());
    }
    SECTION("Conenct")
    {
        EventQueue<int> eventQueue;
        auto conn1 = eventQueue.connect(slot);
        auto conn2 = eventQueue.connect(slot);
        auto conn3 = eventQueue.connect(slot);

        REQUIRE(conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(conn3.isConnected());

        eventQueue.enqueue(42);
        REQUIRE(integers.empty());

        eventQueue.emit();
        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
    }
    SECTION("RecursiveConnection")
    {
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
    SECTION("CallingDisconnect")
    {
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
    SECTION("ArgumentPerfectForwarding")
    {
        EventQueue<std::shared_ptr<int>> eventQueue;
        auto conn = eventQueue.connect([&](std::shared_ptr<int> event) {
            REQUIRE(event != nullptr);
        });
        REQUIRE(conn.isConnected());

        {
            auto pointer = std::make_shared<int>(42);
            REQUIRE(pointer);
            eventQueue.enqueue(pointer);
            REQUIRE(pointer);
        }
        {
            auto pointer = std::make_shared<int>(42);
            REQUIRE(pointer);
            eventQueue.enqueue(std::move(pointer));
            REQUIRE_FALSE(pointer);
        }
    }
}
