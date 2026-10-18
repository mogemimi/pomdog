// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/clipboard/clipboard_service_local.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace clipboard = pomdog::clipboard;

TEST_CASE("ClipboardServiceLocal")
{
    clipboard::ClipboardServiceLocal service;

    SUBCASE("reports its capabilities")
    {
        const auto capabilities = service.getCapabilities();
        REQUIRE(capabilities.writeText == clipboard::FeatureSupport::Available);
        REQUIRE(capabilities.readText == clipboard::FeatureSupport::Available);
        REQUIRE(capabilities.pasteEvents == clipboard::FeatureSupport::Unsupported);
    }
    SUBCASE("reports NoText before the first write")
    {
        const auto request = service.requestReadText();
        REQUIRE(request != 0);
        REQUIRE(service.getEvents().empty());

        service.update();
        const auto events = service.getEvents();
        REQUIRE(events.size() == 1);
        REQUIRE(events[0].kind == clipboard::ClipboardEventKind::ReadCompleted);
        REQUIRE(events[0].requestID == request);
        REQUIRE(events[0].status == clipboard::ClipboardStatus::NoText);
    }
    SUBCASE("reads back what it wrote, with LF line breaks")
    {
        const auto write = service.requestWriteText("a\r\nb");
        const auto read = service.requestReadText();
        REQUIRE(write != read);

        service.update();
        const auto events = service.getEvents();
        REQUIRE(events.size() == 2);
        REQUIRE(events[0].kind == clipboard::ClipboardEventKind::WriteCompleted);
        REQUIRE(events[0].requestID == write);
        REQUIRE(events[0].status == clipboard::ClipboardStatus::Succeeded);
        REQUIRE(events[1].kind == clipboard::ClipboardEventKind::ReadCompleted);
        REQUIRE(events[1].requestID == read);
        REQUIRE(events[1].status == clipboard::ClipboardStatus::Succeeded);
        REQUIRE(events[1].text == "a\nb");
    }
    SUBCASE("reads back empty text as text")
    {
        service.requestWriteText("");
        service.requestReadText();
        service.update();
        const auto events = service.getEvents();
        REQUIRE(events.size() == 2);
        REQUIRE(events[1].status == clipboard::ClipboardStatus::Succeeded);
        REQUIRE(events[1].text.empty());
    }
    SUBCASE("keeps events until the next update")
    {
        service.requestWriteText("a");
        service.update();
        REQUIRE(service.getEvents().size() == 1);
        service.update();
        REQUIRE(service.getEvents().empty());
    }
}
