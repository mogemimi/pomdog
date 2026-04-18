// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/fixed_vector.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::FixedVector;

namespace {

/// A helper type that tracks construction/destruction via an external counter.
/// Every construction (including move) increments the counter; every destruction decrements it.
struct DestructorTracker final {
    int* counter = nullptr;
    explicit DestructorTracker(int* c) noexcept
        : counter(c)
    {
        ++(*counter);
    }
    DestructorTracker(const DestructorTracker& o) noexcept
        : counter(o.counter)
    {
        ++(*counter);
    }
    DestructorTracker(DestructorTracker&& o) noexcept
        : counter(o.counter)
    {
        ++(*counter);
    }
    ~DestructorTracker() noexcept
    {
        --(*counter);
    }

    POMDOG_CLANG_SUPPRESS_WARNING_PUSH
    POMDOG_CLANG_SUPPRESS_WARNING("-Wunneeded-member-function")
    DestructorTracker& operator=(const DestructorTracker&) noexcept = default;
    DestructorTracker& operator=(DestructorTracker&&) noexcept = default;
    POMDOG_CLANG_SUPPRESS_WARNING_POP
};

} // namespace

TEST_CASE("FixedVector")
{
    SUBCASE("push_back and pop_back")
    {
        FixedVector<int, 4> v;
        REQUIRE(v.size() == 0);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v.begin() == v.end());

        v.push_back(100);
        REQUIRE(v.size() == 1);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 100);
        REQUIRE(*v.begin() == 100);

        v.push_back(101);
        REQUIRE(v.size() == 2);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.at(1) == 101);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 101);
        REQUIRE(*v.begin() == 100);

        v.push_back(102);
        REQUIRE(v.size() == 3);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v[2] == 102);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.at(1) == 101);
        REQUIRE(v.at(2) == 102);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 102);
        REQUIRE(*v.begin() == 100);

        v.push_back(103);
        REQUIRE(v.size() == 4);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v[2] == 102);
        REQUIRE(v[3] == 103);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.at(1) == 101);
        REQUIRE(v.at(2) == 102);
        REQUIRE(v.at(3) == 103);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 103);
        REQUIRE(*v.begin() == 100);

        v.pop_back();
        REQUIRE(v.size() == 3);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v[2] == 102);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.at(1) == 101);
        REQUIRE(v.at(2) == 102);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 102);
        REQUIRE(*v.begin() == 100);

        v.pop_back();
        REQUIRE(v.size() == 2);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.at(1) == 101);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 101);
        REQUIRE(*v.begin() == 100);

        v.pop_back();
        REQUIRE(v.size() == 1);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v.at(0) == 100);
        REQUIRE(v.front() == 100);
        REQUIRE(v.back() == 100);
        REQUIRE(*v.begin() == 100);

        v.pop_back();
        REQUIRE(v.size() == 0);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v.begin() == v.end());
    }
    SUBCASE("resize")
    {
        FixedVector<int, 4> v;
        REQUIRE(v.size() == 0);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        v.resize(1);
        REQUIRE(v.size() == 1);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        v.resize(2);
        REQUIRE(v.size() == 2);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        v.resize(3);
        REQUIRE(v.size() == 3);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        v.resize(4);
        REQUIRE(v.size() == 4);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
    }
    SUBCASE("initializer_list with 1 element")
    {
        FixedVector<int, 4> v = {
            100,
        };
        REQUIRE(v.size() == 1);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
    }
    SUBCASE("initializer_list with 2 elements")
    {
        FixedVector<int, 4> v = {100, 101};
        REQUIRE(v.size() == 2);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
    }
    SUBCASE("initializer_list with 3 elements")
    {
        FixedVector<int, 4> v = {100, 101, 102};
        REQUIRE(v.size() == 3);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v[2] == 102);
    }
    SUBCASE("initializer_list with 4 elements")
    {
        FixedVector<int, 4> v = {100, 101, 102, 103};
        REQUIRE(v.size() == 4);
        REQUIRE(v.max_size() == 4);
        REQUIRE(v.capacity() == 4);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v[2] == 102);
        REQUIRE(v[3] == 103);
    }
    SUBCASE("std::swap()")
    {
        FixedVector<int, 4> a = {100, 101, 102};
        FixedVector<int, 4> b = {200, 201};
        REQUIRE(a.size() == 3);
        REQUIRE(a.max_size() == 4);
        REQUIRE(a.capacity() == 4);
        REQUIRE(b.size() == 2);
        REQUIRE(b.max_size() == 4);
        REQUIRE(b.capacity() == 4);
        REQUIRE(a[0] == 100);
        REQUIRE(a[1] == 101);
        REQUIRE(a[2] == 102);
        REQUIRE(b[0] == 200);
        REQUIRE(b[1] == 201);

        std::swap(a, b);
        REQUIRE(b.size() == 3);
        REQUIRE(b.max_size() == 4);
        REQUIRE(b.capacity() == 4);
        REQUIRE(a.size() == 2);
        REQUIRE(a.max_size() == 4);
        REQUIRE(a.capacity() == 4);
        REQUIRE(b[0] == 100);
        REQUIRE(b[1] == 101);
        REQUIRE(b[2] == 102);
        REQUIRE(a[0] == 200);
        REQUIRE(a[1] == 201);
    }
    SUBCASE("pop_back: call destructors")
    {
        auto p = std::make_shared<int>(42);
        REQUIRE(p.use_count() == 1);

        FixedVector<std::shared_ptr<int>, 4> v;
        v.push_back(p);
        REQUIRE(p.use_count() == 2);

        v.push_back(p);
        REQUIRE(p.use_count() == 3);

        v.pop_back();
        REQUIRE(p.use_count() == 2);

        v.pop_back();
        REQUIRE(p.use_count() == 1);
    }
    SUBCASE("erase: range")
    {
        FixedVector<int, 8> v = {100, 101, 102, 103, 104, 105, 106, 107};
        REQUIRE(v.size() == 8);

        v.erase(std::next(std::begin(v), 2), std::next(std::begin(v), 5));
        REQUIRE(v.size() == 5);
        REQUIRE(v[0] == 100);
        REQUIRE(v[1] == 101);
        REQUIRE(v[2] == 105);
        REQUIRE(v[3] == 106);
        REQUIRE(v[4] == 107);
    }
    SUBCASE("erase: call destructors")
    {
        auto p = std::make_shared<int>(42);
        REQUIRE(p.use_count() == 1);

        FixedVector<std::shared_ptr<int>, 4> v;
        v.push_back(p);
        v.push_back(p);
        v.push_back(p);
        v.push_back(p);
        REQUIRE(!v.empty());
        REQUIRE(v.size() == 4);
        REQUIRE(p.use_count() == 5);

        v.erase(v.cbegin(), v.cend());
        REQUIRE(v.empty());
        REQUIRE(v.size() == 0);
        REQUIRE(p.use_count() == 1);
    }
    SUBCASE("clear")
    {
        auto p = std::make_shared<int>(42);
        REQUIRE(p.use_count() == 1);

        FixedVector<std::shared_ptr<int>, 4> v;
        v.push_back(p);
        v.push_back(p);
        v.push_back(p);
        REQUIRE(!v.empty());
        REQUIRE(v.size() == 3);
        REQUIRE(p.use_count() == 4);

        v.clear();
        REQUIRE(v.empty());
        REQUIRE(v.size() == 0);
        REQUIRE(p.use_count() == 1);
    }
    SUBCASE("non-copyable elements")
    {
        FixedVector<std::unique_ptr<int>, 4> v;
        v.push_back(std::make_unique<int>(42));
        v.push_back(std::make_unique<int>(42));
        REQUIRE(!v.empty());
        REQUIRE(v.size() == 2);

        v.clear();
        REQUIRE(v.empty());
        REQUIRE(v.size() == 0);
    }
    SUBCASE("swap: non-copyable elements")
    {
        FixedVector<std::unique_ptr<int>, 4> v;
        FixedVector<std::unique_ptr<int>, 4> w;
        v.push_back(std::make_unique<int>(42));
        v.push_back(std::make_unique<int>(42));
        REQUIRE(!v.empty());
        REQUIRE(v.size() == 2);
        REQUIRE(w.empty());
        REQUIRE(w.size() == 0);

        std::swap(v, w);
        REQUIRE(v.empty());
        REQUIRE(v.size() == 0);
        REQUIRE(!w.empty());
        REQUIRE(w.size() == 2);
    }
    SUBCASE("large size")
    {
        FixedVector<int, 512> v;
        for (int i = 0; i < static_cast<int>(v.capacity()); i++) {
            v.push_back(i);
        }
        REQUIRE(!v.empty());
        REQUIRE(v.size() == 512);
    }
    SUBCASE("resize: shrink")
    {
        // NOTE: Bug case: resize off-by-one
        FixedVector<int, 8> v = {10, 20, 30, 40, 50};
        REQUIRE(v.size() == 5);

        v.resize(3);
        REQUIRE(v.size() == 3);
        REQUIRE(v[0] == 10);
        REQUIRE(v[1] == 20);
        REQUIRE(v[2] == 30);

        v.resize(1);
        REQUIRE(v.size() == 1);
        REQUIRE(v[0] == 10);
    }
    SUBCASE("operator== with different sizes")
    {
        // NOTE: Bug case: operator== doesn't compare sizes
        FixedVector<int, 4> a = {1, 2};
        FixedVector<int, 4> b = {1, 2, 3};
        REQUIRE(a.size() != b.size());
        REQUIRE(!(a == b));
        REQUIRE(a != b);
    }
    SUBCASE("swap: different sizes asymmetric")
    {
        // NOTE: Bug case: swap doesn't handle different sizes correctly
        FixedVector<int, 4> a = {10};
        FixedVector<int, 4> b = {1, 2, 3};
        a.swap(b);
        REQUIRE(a.size() == 3);
        REQUIRE(b.size() == 1);
        REQUIRE(a[0] == 1);
        REQUIRE(a[1] == 2);
        REQUIRE(a[2] == 3);
        REQUIRE(b[0] == 10);
    }
    SUBCASE("self-assignment")
    {
        // NOTE: Bug case: copy assignment not safe for self-assignment
        FixedVector<int, 4> a = {1, 2, 3};
        auto& ref = a;
        a = ref;
        REQUIRE(a.size() == 3);
        REQUIRE(a[0] == 1);
        REQUIRE(a[1] == 2);
        REQUIRE(a[2] == 3);
    }
    SUBCASE("move constructor: destructor tracking")
    {
        // NOTE: Bug case: move constructor doesn't call destructors on moved-from elements
        int count = 0;
        {
            FixedVector<DestructorTracker, 4> a;
            a.emplace_back(&count);
            a.emplace_back(&count);
            REQUIRE(count == 2);

            FixedVector<DestructorTracker, 4> b(std::move(a));
            // NOTE: Move constructs 2 new, then clear() destroys 2 moved-from -> net count = 2
            REQUIRE(count == 2);
        }
        // NOTE: All must be destructed
        REQUIRE(count == 0);
    }
    SUBCASE("move assignment: destructor tracking")
    {
        // NOTE: Bug case: move assignment doesn't call destructors on moved-from elements
        int count = 0;
        {
            FixedVector<DestructorTracker, 4> a;
            a.emplace_back(&count);
            a.emplace_back(&count);
            REQUIRE(count == 2);

            FixedVector<DestructorTracker, 4> b;
            b = std::move(a);
            // NOTE: Move constructs 2 new, then clear() destroys 2 moved-from -> net count = 2
            REQUIRE(count == 2);
        }
        REQUIRE(count == 0);
    }
    SUBCASE("data: empty vector")
    {
        // NOTE: Bug case: data() should not invoke UB on empty vector
        FixedVector<int, 4> v;
        REQUIRE(v.empty());
        auto ptr = v.data();
        REQUIRE(ptr != nullptr);
    }
    SUBCASE("operator= initializer_list")
    {
        // NOTE: Bug case: operator=(initializer_list) was passing x instead of e
        FixedVector<int, 4> v;
        v.push_back(999);
        v = {10, 20, 30};
        REQUIRE(v.size() == 3);
        REQUIRE(v[0] == 10);
        REQUIRE(v[1] == 20);
        REQUIRE(v[2] == 30);
    }
    SUBCASE("std::get rvalue")
    {
        // NOTE: Bug case: std::get rvalue overloads used non-existent member
        FixedVector<int, 4> v = {100, 200, 300};
        auto val = std::get<1>(std::move(v));
        REQUIRE(val == 200);
    }
}
