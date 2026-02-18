// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/circular_buffer.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::CircularBuffer;

TEST_CASE("CircularBuffer")
{
    CircularBuffer<int> buffer(5);
    REQUIRE(buffer.empty());
    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.max_size() == 5);
    REQUIRE(buffer.capacity() == 5);

    buffer.push_back(100);
    REQUIRE(!buffer.empty());
    REQUIRE(buffer.size() == 1);
    REQUIRE(buffer.max_size() == 5);
    REQUIRE(buffer.capacity() == 5);
    REQUIRE(buffer[0] == 100);

    buffer.push_back(101);
    REQUIRE(!buffer.empty());
    REQUIRE(buffer.size() == 2);
    REQUIRE(buffer.max_size() == 5);
    REQUIRE(buffer.capacity() == 5);
    REQUIRE(buffer[0] == 100);
    REQUIRE(buffer[1] == 101);

    buffer.push_back(102);
    REQUIRE(!buffer.empty());
    REQUIRE(buffer.size() == 3);
    REQUIRE(buffer.max_size() == 5);
    REQUIRE(buffer.capacity() == 5);
    REQUIRE(buffer[0] == 100);
    REQUIRE(buffer[1] == 101);
    REQUIRE(buffer[2] == 102);

    buffer.push_back(103);
    REQUIRE(!buffer.empty());
    REQUIRE(buffer.size() == 4);
    REQUIRE(buffer.max_size() == 5);
    REQUIRE(buffer.capacity() == 5);
    REQUIRE(buffer[0] == 100);
    REQUIRE(buffer[1] == 101);
    REQUIRE(buffer[2] == 102);
    REQUIRE(buffer[3] == 103);

    buffer.push_back(104);
    REQUIRE(!buffer.empty());
    REQUIRE(buffer.size() == 5);
    REQUIRE(buffer.max_size() == 5);
    REQUIRE(buffer.capacity() == 5);
    REQUIRE(buffer[0] == 100);
    REQUIRE(buffer[1] == 101);
    REQUIRE(buffer[2] == 102);
    REQUIRE(buffer[3] == 103);
    REQUIRE(buffer[4] == 104);

    SUBCASE("pop_back()")
    {
        buffer.pop_back();
        REQUIRE(!buffer.empty());
        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.max_size() == 5);
        REQUIRE(buffer.capacity() == 5);
        REQUIRE(buffer[0] == 100);
        REQUIRE(buffer[1] == 101);
        REQUIRE(buffer[2] == 102);
        REQUIRE(buffer[3] == 103);
    }
    SUBCASE("pop_back()")
    {
        buffer.pop_back();
        buffer.pop_back();
        REQUIRE(!buffer.empty());
        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer.max_size() == 5);
        REQUIRE(buffer.capacity() == 5);
        REQUIRE(buffer[0] == 100);
        REQUIRE(buffer[1] == 101);
        REQUIRE(buffer[2] == 102);
    }
    SUBCASE("pop_back()")
    {
        buffer.pop_back();
        buffer.pop_back();
        buffer.pop_back();
        buffer.pop_back();
        buffer.pop_back();
        REQUIRE(buffer.empty());
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.max_size() == 5);
        REQUIRE(buffer.capacity() == 5);
    }
    SUBCASE("pop_front()")
    {
        buffer.pop_front();
        REQUIRE(!buffer.empty());
        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.max_size() == 5);
        REQUIRE(buffer.capacity() == 5);
        REQUIRE(buffer[0] == 101);
        REQUIRE(buffer[1] == 102);
        REQUIRE(buffer[2] == 103);
        REQUIRE(buffer[3] == 104);
    }
    SUBCASE("pop_front()")
    {
        buffer.pop_front();
        buffer.pop_front();
        REQUIRE(!buffer.empty());
        REQUIRE(buffer.size() == 3);
        REQUIRE(buffer.max_size() == 5);
        REQUIRE(buffer.capacity() == 5);
        REQUIRE(buffer[0] == 102);
        REQUIRE(buffer[1] == 103);
        REQUIRE(buffer[2] == 104);
    }
    SUBCASE("pop_front()")
    {
        buffer.pop_front();
        buffer.pop_front();
        buffer.pop_front();
        buffer.pop_front();
        buffer.pop_front();
        REQUIRE(buffer.empty());
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.max_size() == 5);
        REQUIRE(buffer.capacity() == 5);
    }
}
