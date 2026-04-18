// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/application/backends/system_event_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <any>
#include <functional>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::detail::InputTextEvent;
using pomdog::detail::SystemEvent;
using pomdog::detail::SystemEventKind;
using pomdog::detail::SystemEventQueue;

TEST_CASE("EventQueue")
{
    SUBCASE("enqueue")
    {
        SystemEventQueue queue = {};

        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "Hello"},
        });

        std::string result = {};
        queue.emit([&result](SystemEvent event) {
            if (event.kind == SystemEventKind::InputTextEvent) {
                const auto e = std::get_if<InputTextEvent>(&event.data);
                REQUIRE(e != nullptr);
                result = e->text;
            }
        });

        REQUIRE(result == "Hello");
    }
    SUBCASE("enqueue multiple")
    {
        SystemEventQueue queue = {};
        std::vector<std::string> results;

        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "Hello"},
        });
        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "World"},
        });
        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "!"}});

        queue.emit([&results](SystemEvent event) {
            if (event.kind == SystemEventKind::InputTextEvent) {
                const auto e = std::get_if<InputTextEvent>(&event.data);
                REQUIRE(e != nullptr);
                results.push_back(e->text);
            }
        });

        REQUIRE(results.size() == 3);
        REQUIRE(results[0] == "Hello");
        REQUIRE(results[1] == "World");
        REQUIRE(results[2] == "!");
    }
    SUBCASE("clear queue")
    {
        SystemEventQueue queue;
        std::vector<std::string> results;

        const auto func = [&results](SystemEvent event) {
            if (event.kind == SystemEventKind::InputTextEvent) {
                const auto e = std::get_if<InputTextEvent>(&event.data);
                REQUIRE(e != nullptr);
                results.push_back(e->text);
            }
        };

        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "Hello"},
        });
        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "World"},
        });

        queue.emit(func);

        REQUIRE(results.size() == 2);

        queue.emit(func);

        REQUIRE(results.size() == 2);
    }
    SUBCASE("recursive calls to emit")
    {
        SystemEventQueue queue;
        std::vector<std::string> results;

        const auto func = [&queue, &results](SystemEvent event) {
            if (event.kind == SystemEventKind::InputTextEvent) {
                const auto e = std::get_if<InputTextEvent>(&event.data);
                REQUIRE(e != nullptr);
                results.push_back(e->text);

                if (e->text == "Hello") {
                    queue.enqueue(SystemEvent{
                        .kind = SystemEventKind::InputTextEvent,
                        .data = InputTextEvent{.text = "World"},
                    });
                }
            }
        };

        queue.enqueue(SystemEvent{
            .kind = SystemEventKind::InputTextEvent,
            .data = InputTextEvent{.text = "Hello"},
        });

        queue.emit(func);

        REQUIRE(results.size() == 1);
        REQUIRE(results[0] == "Hello");

        queue.emit(func);

        REQUIRE(results.size() == 2);
        REQUIRE(results[0] == "Hello");
        REQUIRE(results[1] == "World");
    }
}
