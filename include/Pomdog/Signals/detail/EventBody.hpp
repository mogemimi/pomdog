// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <type_traits>
#include <typeinfo>
#include <utility>

namespace Pomdog {
namespace Detail {

template <class T>
struct EventHashCode final {
    static_assert(!std::is_pointer<T>::value, "T is not pointer.");
    static_assert(std::is_object<T>::value, "T is not object type.");

    static const std::size_t value;
};

template <class T>
const std::size_t EventHashCode<T>::value = typeid(const T*).hash_code();

class EventBody {
public:
    EventBody() = default;
    EventBody(const EventBody&) = delete;
    EventBody& operator=(const EventBody&) = delete;

    virtual ~EventBody() = default;

    virtual std::size_t HashCode() const noexcept = 0;
};

template <typename T>
class EventBodyOverride final : public EventBody {
public:
    static_assert(!std::is_reference<T>::value,
        "reference type is not supported.");
    static_assert(!std::is_pointer<T>::value,
        "pointer type is not supported.");
    static_assert(std::is_object<T>::value,
        "T is object type.");

    template <typename... Arguments>
    explicit EventBodyOverride(Arguments&&... argument)
        : data(std::forward<Arguments>(argument)...)
    {
    }

    std::size_t HashCode() const noexcept override
    {
        return EventHashCode<T>::value;
    }

    T data;
};

} // namespace Detail
} // namespace Pomdog
