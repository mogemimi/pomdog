// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/detail/CRC32.hpp"
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>

namespace Pomdog {
namespace Detail {

// TypeIdHashing provides typeid/type_info comparison across shared library boundaries.
template <class T>
struct POMDOG_EXPORT TypeIdHashing final {
    static const std::uint32_t hashCode;
};

template <class T>
const std::uint32_t TypeIdHashing<T>::hashCode
    = Detail::CRC32::ComputeCRC32<char>(typeid(T).name());

} // namespace Detail

class POMDOG_EXPORT Any final {
private:
    struct HolderBase {
        virtual ~HolderBase() = default;
    };

    template <typename T>
    struct Holder final : public HolderBase {
        T Value;

        template <typename U>
        explicit Holder(U && valueIn)
            : Value(std::forward<U>(valueIn))
        {
        }

        static_assert(std::is_object<T>::value, "");
    };

    std::unique_ptr<HolderBase> data;
    std::type_index typeIndex;
    std::uint32_t hashCode;

public:
    Any() = delete;
    Any(const Any&) = delete;
    Any(Any &&) = default;

    Any& operator=(const Any&) = delete;
    Any& operator=(Any &&) = default;

    template <typename T>
    Any(T && value)
        : data(std::make_unique<Holder<typename std::remove_reference<T>::type>>(std::forward<T>(value)))
        , typeIndex(typeid(T))
        , hashCode(Detail::TypeIdHashing<T>::hashCode)
    {
    }

    template <typename T>
    bool Is() const
    {
        return typeIndex == typeid(T)
            || hashCode == Detail::TypeIdHashing<T>::hashCode;
    }

    template <typename T>
    const T& As() const
    {
        POMDOG_ASSERT(typeIndex == typeid(T)
            || hashCode == Detail::TypeIdHashing<T>::hashCode);

        if (!Is<T>()) {
            //throw BadAnyCast;
        }

        POMDOG_ASSERT(data);
        auto derived = dynamic_cast<Holder<T>*>(data.get());
        POMDOG_ASSERT(derived);
        return derived->Value;
    }

    template <typename T>
    T & As()
    {
        POMDOG_ASSERT(typeIndex == typeid(T)
            || hashCode == Detail::TypeIdHashing<T>::hashCode);

        if (!Is<T>()) {
            //throw BadAnyCast;
        }

        POMDOG_ASSERT(data);
        auto derived = dynamic_cast<Holder<T>*>(data.get());
        POMDOG_ASSERT(derived);
        return derived->Value;
    }

    std::type_index Type() const
    {
        return typeIndex;
    }
};

} // namespace Pomdog
