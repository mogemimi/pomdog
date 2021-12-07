// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// ArrayView represents a view into the contiguous sequence data held in a buffer.
template <typename T>
class POMDOG_EXPORT ArrayView final {
    T* data = nullptr;
    std::size_t size = 0;

public:
    ArrayView() = default;

    ArrayView(T* dataIn, size_t sizeIn)
        : data(dataIn)
        , size(sizeIn)
    {
    }

    /// Returns a pointer to the first element of a view.
    [[nodiscard]] T* GetData() const noexcept
    {
        return data;
    }

    /// Returns the number of elements.
    [[nodiscard]] std::size_t GetSize() const noexcept
    {
        return size;
    }

    /// Returns true if the array is empty, false otherwise.
    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return size <= 0;
    }

    /// Gets the last element of a view.
    [[nodiscard]] const T& GetBack() const
    {
        static_assert(!std::is_void_v<std::remove_const_t<T>>);
        POMDOG_ASSERT(size > 0);
        POMDOG_ASSERT(data != nullptr);
        return data[size - 1];
    }

    template <typename R>
    [[nodiscard]] ArrayView<R> ViewAs() noexcept
    {
        static_assert(!std::is_same_v<T, R>);
        constexpr auto t = std::is_void_v<T> ? 1 : sizeof(T);
        constexpr auto r = std::is_void_v<R> ? 1 : sizeof(R);
        static_assert(t > 0);
        static_assert(r > 0);
        static_assert(t >= r);
        constexpr auto scale = (t / r);
        static_assert(scale > 0);
        const auto s = size * scale;
        ArrayView<R> view{reinterpret_cast<R*>(data), s};
        return view;
    }
};

} // namespace Pomdog
