// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_EVENT_56ECB270_HPP
#define POMDOG_EVENT_56ECB270_HPP

#include "detail/EventBody.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <type_traits>
#include <utility>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT Event {
public:
    Event() = delete;
    Event(Event const&) = delete;
    Event(Event &&) = default;

    Event& operator=(Event const&) = delete;
    Event& operator=(Event &&) = default;

    template <typename T>
    explicit Event(T && value)
    {
        typedef typename std::remove_reference<T>::type valueType;
        typedef Detail::EventBodyOverride<valueType> containerType;

        static_assert(!std::is_same<Event, valueType>::value,
            "T is not Event type.");
        static_assert(std::is_object<valueType>::value,
            "T is object type.");
        static_assert(!std::is_pointer<valueType>::value,
            "pointer type is not supported.");
        static_assert(std::is_base_of<Detail::EventBody, containerType>::value,
            "Container is not a base class of 'EventBody'");

        body = std::make_unique<containerType>(std::forward<T>(value));
    }

    template <typename T>
    bool Is() const
    {
        static_assert(!std::is_reference<T>::value,
            "reference type is not supported.");
        static_assert(!std::is_pointer<T>::value,
            "pointer type is not supported.");
        static_assert(std::is_object<T>::value,
            "T is object type.");

        POMDOG_ASSERT(body);
        return body && (body->HashCode() == Detail::EventHashCode<T>::value);
    }

    template <typename T>
    T const* As() const
    {
        typedef Detail::EventBodyOverride<T> containerType;

        static_assert(!std::is_reference<T>::value,
            "reference type is not supported.");
        static_assert(!std::is_pointer<T>::value,
            "pointer type is not supported.");
        static_assert(std::is_object<T>::value,
            "T is object type.");
        static_assert(std::is_base_of<Detail::EventBody, containerType>::value,
            "T is not a base class of 'EventBody'.");

        POMDOG_ASSERT(body);

        if (auto p = dynamic_cast<containerType const*>(body.get())) {
            POMDOG_ASSERT(Is<T>());
            return &p->data;
        }
        return nullptr;
    }

private:
    std::unique_ptr<Detail::EventBody> body;
};

} // namespace Pomdog

#endif // POMDOG_EVENT_56ECB270_HPP
