// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/EventQueue.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "catch.hpp"
#include <any>
#include <utility>

using Pomdog::Connection;
using Pomdog::EventQueue;

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
        auto connection = eventQueue.Connect(slot);

        eventQueue.Enqueue(42);
        eventQueue.Enqueue(43);
        eventQueue.Enqueue(44);
        REQUIRE(integers.empty());

        eventQueue.Emit();
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
        auto conn = eventQueue.Connect([&](const std::any& event) {
            REQUIRE(std::any_cast<User>(&event) != nullptr);
            auto user = std::any_cast<User>(&event);
            names.push_back(user->name);
            integers.push_back(user->id);
        });
        REQUIRE(conn.IsConnected());

        eventQueue.Enqueue(std::make_any<User>(User{.name = "Donald", .id = 42}));
        eventQueue.Enqueue(std::make_any<User>(User{.name = "Goofy", .id = 43}));
        REQUIRE(names.empty());
        REQUIRE(integers.empty());

        eventQueue.Emit();
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
        auto connection = eventQueue.Connect(slot);

        eventQueue.Enqueue(42);
        REQUIRE(integers.empty());

        connection.Disconnect();
        eventQueue.Emit();
        REQUIRE(integers.empty());
    }
    SECTION("Conenct")
    {
        EventQueue<int> eventQueue;
        auto conn1 = eventQueue.Connect(slot);
        auto conn2 = eventQueue.Connect(slot);
        auto conn3 = eventQueue.Connect(slot);

        REQUIRE(conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(conn3.IsConnected());

        eventQueue.Enqueue(42);
        REQUIRE(integers.empty());

        eventQueue.Emit();
        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
    }
    SECTION("RecursiveConnection")
    {
        EventQueue<int> eventQueue;
        auto conn = eventQueue.Connect([&](int) {
            auto conn2 = eventQueue.Connect(slot);
            REQUIRE(conn2.IsConnected());
        });
        REQUIRE(conn.IsConnected());

        eventQueue.Enqueue(42);
        REQUIRE(integers.empty());
        eventQueue.Emit();
        REQUIRE(integers.empty());

        eventQueue.Enqueue(43);
        REQUIRE(integers.empty());

        eventQueue.Emit();
        REQUIRE(integers.size() == 1);
        REQUIRE(integers[0] == 43);
    }
    SECTION("CallingDisconnect")
    {
        EventQueue<int> eventQueue;
        Connection conn;
        REQUIRE(!conn.IsConnected());
        conn = eventQueue.Connect([&](int event) {
            slot(event);
            REQUIRE(conn.IsConnected());
            conn.Disconnect();
            REQUIRE(!conn.IsConnected());
        });
        REQUIRE(conn.IsConnected());

        eventQueue.Enqueue(42);
        REQUIRE(conn.IsConnected());
        REQUIRE(integers.empty());
        eventQueue.Emit();
        REQUIRE(!conn.IsConnected());
        REQUIRE(integers.size() == 1);
        REQUIRE(integers[0] == 42);

        integers.clear();
        eventQueue.Enqueue(43);
        REQUIRE(integers.empty());
        eventQueue.Emit();
        REQUIRE(integers.empty());
    }
    SECTION("ArgumentPerfectForwarding")
    {
        EventQueue<std::shared_ptr<int>> eventQueue;
        auto conn = eventQueue.Connect([&](std::shared_ptr<int> event) {
            REQUIRE(event != nullptr);
        });
        REQUIRE(conn.IsConnected());

        {
            auto pointer = std::make_shared<int>(42);
            REQUIRE(pointer);
            eventQueue.Enqueue(pointer);
            REQUIRE(pointer);
        }
        {
            auto pointer = std::make_shared<int>(42);
            REQUIRE(pointer);
            eventQueue.Enqueue(std::move(pointer));
            REQUIRE_FALSE(pointer);
        }
    }
}
