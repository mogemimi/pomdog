// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// ArrayView represents a view into the contiguous sequence data held in a buffer.
template <typename T>
class POMDOG_EXPORT ArrayView final {
    T* data_ = nullptr;
    std::size_t size_ = 0;

public:
    ArrayView() = default;

    ArrayView(T* dataIn, size_t sizeIn)
        : data_(dataIn)
        , size_(sizeIn)
    {
    }

    /// Returns a pointer to the first element of a view.
    [[nodiscard]] T* data() const noexcept
    {
        return data_;
    }

    /// Returns the number of elements.
    [[nodiscard]] std::size_t size() const noexcept
    {
        return size_;
    }

    /// Returns true if the array is empty, false otherwise.
    [[nodiscard]] bool empty() const noexcept
    {
        return size_ <= 0;
    }

    /// Gets the last element of a view.
    [[nodiscard]] const T& back() const
    {
        static_assert(!std::is_void_v<std::remove_const_t<T>>);
        POMDOG_ASSERT(size_ > 0);
        POMDOG_ASSERT(data_ != nullptr);
        return data_[size_ - 1];
    }

    template <typename R>
    [[nodiscard]] ArrayView<R> viewAs() noexcept
    {
        static_assert(!std::is_same_v<T, R>);
        constexpr auto t = std::is_void_v<T> ? 1 : sizeof(T);
        constexpr auto r = std::is_void_v<R> ? 1 : sizeof(R);
        static_assert(t > 0);
        static_assert(r > 0);
        static_assert(t >= r);
        constexpr auto scale = (t / r);
        static_assert(scale > 0);
        const auto s = size_ * scale;
        ArrayView<R> view{reinterpret_cast<R*>(data_), s};
        return view;
    }
};

} // namespace pomdog
