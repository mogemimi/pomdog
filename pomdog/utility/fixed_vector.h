// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/aligned_byte_array.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Represents a vector-like container with a fixed capacity. It does not perform dynamic memory allocation.
/// The size of the vector can grow up to the specified capacity, but it cannot exceed it.
template <typename Type, std::size_t Size>
class FixedVector final {
public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using storage_type = detail::AlignedByteArray<alignof(value_type), sizeof(value_type)>;

    static_assert(Size > 0);
    static_assert(std::is_standard_layout<storage_type>::value);

private:
    storage_type storage_[Size];
    size_type size_ = 0;

public:
    /// Default constructor. Initializes an empty vector with a size of 0.
    constexpr FixedVector() noexcept = default;

    /// Copy constructor. Creates a new vector as a copy of the given vector `rhs`.
    /// The new vector will have the same size and elements as `rhs`.
    FixedVector(const FixedVector& rhs) noexcept(
        std::is_nothrow_copy_constructible_v<value_type>)
    {
        for (auto& e : rhs) {
            push_back(e);
        }
    }

    /// Move constructor. Creates a new vector by moving the elements from the given vector `rhs`.
    /// The new vector will have the same size and elements as `rhs`, and `rhs` will be left in an empty state.
    FixedVector(FixedVector&& rhs) noexcept(
        std::is_nothrow_move_constructible_v<value_type>)
    {
        for (auto& e : rhs) {
            push_back(std::move(e));
        }
        rhs.size_ = 0;
    }

    /// Constructor that initializes the vector with elements from an initializer list.
    /// The new vector will contain the elements from the initializer list,
    /// and its size will be equal to the number of elements in the list.
    template <typename T>
    FixedVector(std::initializer_list<T> rhs) noexcept(
        std::is_nothrow_move_constructible_v<Type> && std::is_nothrow_move_assignable_v<T>)
    {
        for (auto& e : rhs) {
            emplace_back(std::move(e));
        }
    }

    POMDOG_MSVC_SUPPRESS_WARNING_PUSH
    POMDOG_MSVC_SUPPRESS_WARNING(4710)
    /// Destructor. Destroys all elements in the vector and releases any resources held by the vector.
    ~FixedVector() noexcept(
        std::is_nothrow_destructible_v<value_type>)
    {
        destroy_range(begin(), end());
    }
    POMDOG_MSVC_SUPPRESS_WARNING_POP

    /// Swaps the contents of this vector with another vector `rhs`.
    /// This operation is noexcept if the element type is nothrow swappable.
    constexpr void swap(FixedVector& rhs) noexcept(
        std::is_nothrow_swappable_v<value_type>)
    {
        std::swap_ranges(begin(), end(), rhs.begin());
        std::swap(size_, rhs.size_);
    }

    /// Copy assignment operator. Assigns the contents of `x` to this vector.
    /// The vector will be resized to match the size of `x`, and the elements will be copied from `x` to this vector.
    FixedVector& operator=(const FixedVector& x)
    {
        clear();
        for (auto& e : x) {
            push_back(e);
        }
        return *this;
    }

    /// Move assignment operator. Moves the contents of `x` to this vector.
    /// The vector will be resized to match the size of `x`, and the elements
    /// will be moved from `x` to this vector. After the operation, `x` will be left in an empty state.
    FixedVector& operator=(FixedVector&& x) noexcept(
        std::is_nothrow_move_assignable_v<value_type> && std::is_nothrow_destructible_v<value_type>)
    {
        clear();
        for (auto& e : x) {
            push_back(std::move(e));
        }
        x.size_ = 0;
        return *this;
    }

    /// Copy assignment operator for vectors with different capacities. Assigns the contents of `x` to this vector.
    /// The vector will be resized to match the size of `x`, and the elements will be copied from `x` to this vector.
    /// This operation is only valid if the size of `x` does not exceed the capacity of this vector.
    template <size_type RhsCapactity>
    FixedVector& operator=(const FixedVector<value_type, RhsCapactity>& x)
    {
        POMDOG_ASSERT(x.size() <= capacity());
        clear();
        for (auto& e : x) {
            push_back(e);
        }
        return *this;
    }

    /// Move assignment operator for vectors with different capacities. Moves the contents of `x` to this vector.
    /// The vector will be resized to match the size of `x`, and the elements will be moved from `x` to this vector.
    /// After the operation, `x` will be left in an empty state.
    /// This operation is only valid if the size of `x` does not exceed the capacity of this vector.
    template <size_type RhsCapactity>
    typename std::enable_if<RhsCapactity <= Size, FixedVector&>::type
    operator=(FixedVector<value_type, RhsCapactity>&& x) noexcept(
        std::is_nothrow_move_assignable_v<value_type> && std::is_nothrow_destructible_v<value_type>)
    {
        static_assert(x.capacity() <= capacity());
        POMDOG_ASSERT(x.size() <= capacity());
        clear();
        for (auto& e : x) {
            push_back(std::move(e));
        }
        x._size = 0;
        return *this;
    }

    /// Assignment operator for initializer lists. Assigns the contents of the initializer list `x` to this vector.
    /// The vector will be resized to match the number of elements in the initializer list, and the elements will be copied from the list to this vector.
    /// This operation is only valid if the number of elements in the initializer list does not exceed the capacity of this vector.
    template <typename T>
    FixedVector& operator=(std::initializer_list<T> x)
    {
        POMDOG_ASSERT(x.size() <= capacity());
        clear();
        for (auto& e : x) {
            emplace_back(std::forward<T>(x));
        }
        return *this;
    }

    /// Returns an iterator to the beginning of the vector.
    /// The iterator points to the first element in the vector, or to the end if the vector is empty.
    constexpr iterator begin() noexcept
    {
        return iterator(reinterpret_cast<pointer>(storage_));
    }

    /// Returns a const iterator to the beginning of the vector.
    /// The iterator points to the first element in the vector, or to the end if the vector is empty.
    constexpr const_iterator begin() const noexcept
    {
        return const_iterator(reinterpret_cast<const_pointer>(storage_));
    }

    /// Returns a const iterator to the beginning of the vector.
    /// The iterator points to the first element in the vector, or to the end if the vector is empty.
    constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(reinterpret_cast<const_pointer>(storage_));
    }

    /// Returns an iterator to the end of the vector.
    /// The iterator points to the position just past the last element in the vector.
    constexpr iterator end() noexcept
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        return begin() + size();
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    /// Returns a const iterator to the end of the vector.
    /// The iterator points to the position just past the last element in the vector.
    constexpr const_iterator end() const noexcept
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        return begin() + size();
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    /// Returns a const iterator to the end of the vector.
    /// The iterator points to the position just past the last element in the vector.
    constexpr const_iterator cend() const noexcept
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        return cbegin() + size();
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    /// Returns the number of elements in the vector. The size is always less than or equal to the capacity.
    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return size_;
    }

    /// Returns the maximum number of elements that the vector can hold, which is equal to the template parameter `Size`.
    [[nodiscard]] constexpr size_type max_size() const noexcept
    {
        return Size;
    }

    /// Returns the capacity of the vector, which is equal to the template parameter `Size`.
    [[nodiscard]] constexpr size_type capacity() const noexcept
    {
        return Size;
    }

    /// Returns true if the vector is empty (i.e., contains no elements), and false otherwise.
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size_ == 0;
    }

    /// Clears the vector, destroying all elements.
    /// After calling this function, the size of the vector will be 0, but the capacity remains unchanged.
    void clear() noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        for (auto& elem : *this) {
            elem.~value_type();
        }
        size_ = 0;
    }

    /// Returns a reference to the element at the specified index.
    [[nodiscard]] constexpr reference operator[](size_type index) noexcept
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        return (reinterpret_cast<pointer>(storage_))[index];
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    /// Returns a const reference to the element at the specified index.
    [[nodiscard]] constexpr const_reference operator[](size_type index) const noexcept
    {
        return (reinterpret_cast<const_pointer>(storage_))[index];
    }

    /// Returns a reference to the element at the specified index with bounds checking.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(NDEBUG)
    // NOTE: For avoiding the C5045 warning about "Spectre Attacks"
    __declspec(spectre(nomitigation)) constexpr reference at(size_type index)
#else
    constexpr reference at(size_type index)
#endif
    {
        if (index >= size_) [[unlikely]] {
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__)
            std::__throw_out_of_range("FixedVector::at");
#elif defined(_MSC_VER)
            std::_Xout_of_range("FixedVector::at");
#else
            throw std::range_error("FixedVector::at");
#endif
        }
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        return (reinterpret_cast<pointer>(storage_))[index];
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    /// Returns a const reference to the element at the specified index with bounds checking.
    [[nodiscard]] constexpr const_reference at(size_type index) const
    {
        if (index >= size_) [[unlikely]] {
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__)
            std::__throw_out_of_range("FixedVector::at");
#elif defined(_MSC_VER)
            std::_Xout_of_range("FixedVector::at");
#else
            throw std::range_error("FixedVector::at");
#endif
        }
        return (reinterpret_cast<const_pointer>(storage_))[index];
    }

    /// Returns a reference to the first element in the vector.
    [[nodiscard]] constexpr reference front() noexcept
    {
        return (*this)[0];
    }

    /// Returns a const reference to the first element in the vector.
    [[nodiscard]] constexpr const_reference front() const noexcept
    {
        return (*this)[0];
    }

    /// Returns a reference to the last element in the vector.
    [[nodiscard]] constexpr reference back() noexcept
    {
        return (*this)[size_ - 1];
    }

    /// Returns a const reference to the last element in the vector.
    [[nodiscard]] constexpr const_reference back() const noexcept
    {
        return (*this)[size_ - 1];
    }

    /// Returns a pointer to the underlying array serving as element storage.
    [[nodiscard]] constexpr pointer data() noexcept
    {
        return std::addressof(front());
    }

    /// Returns a const pointer to the underlying array serving as element storage.
    [[nodiscard]] constexpr const_pointer data() const noexcept
    {
        return std::addressof(front());
    }

    POMDOG_MSVC_SUPPRESS_WARNING_PUSH
    POMDOG_MSVC_SUPPRESS_WARNING(5045)
    /// Appends the given element to the end of the vector.
    /// If the vector is already at its maximum capacity, this function will throw an exception.
    void push_back(const value_type& element)
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        grow(size_ + 1);
        new (storage_ + (size_ - 1)) value_type(element);
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    POMDOG_MSVC_SUPPRESS_WARNING_POP

    POMDOG_MSVC_SUPPRESS_WARNING_PUSH
    POMDOG_MSVC_SUPPRESS_WARNING(5045)
    /// Appends the given element to the end of the vector.
    /// If the vector is already at its maximum capacity, this function will throw an exception.
    void push_back(value_type&& element)
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        grow(size_ + 1);
        new (storage_ + (size_ - 1)) value_type(std::forward<value_type>(element));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    POMDOG_MSVC_SUPPRESS_WARNING_POP

    template <class... Args>
    /// Constructs an element in-place at the end of the vector using the provided arguments.
    /// If the vector is already at its maximum capacity, this function will throw an exception.
    reference emplace_back(Args&&... args)
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        grow(size_ + 1);
        new (storage_ + (size_ - 1)) value_type(std::forward<Args>(args)...);
        return back();
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    POMDOG_MSVC_SUPPRESS_WARNING_PUSH
    POMDOG_MSVC_SUPPRESS_WARNING(4710)
    /// Removes the last element from the vector.
    /// The size of the vector will be reduced by one after this operation.
    /// If the vector is empty, this function will throw an exception.
    void pop_back()
    {
        if (size_ <= 0) [[unlikely]] {
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__)
            std::__throw_out_of_range("FixedVector::pop_back");
#elif defined(_MSC_VER)
            std::_Xout_of_range("FixedVector::pop_back");
#else
            throw std::range_error("FixedVector::pop_back");
#endif
        }

        --size_;
        this->end()->~value_type();
    }
    POMDOG_MSVC_SUPPRESS_WARNING_POP

    /// Erases the element at the specified position from the vector.
    /// The elements after the erased element will be moved to fill the gap.
    /// The size of the vector will be reduced by one after this operation.
    iterator erase(const_iterator position) noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        return erase(position, position + 1);
    }

    /// Erases the elements in the range [first, last) from the vector.
    /// The elements in the range [last, end()) will be moved to the position starting
    /// from `first` to fill the gap left by the erased elements. After this operation,
    /// the size of the vector will be reduced by the number of erased elements.
    iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        POMDOG_ASSERT(first <= last);
        pointer p = data() + (first - begin());
        if (first != last) {
            destruct_at_end(std::move(make_iter(p + (last - first)), end(), make_iter(p)));
        }
        iterator r = make_iter(p);
        return r;
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    /// Resizes the vector to contain `n` elements.
    ///
    /// If `n` is smaller than the current size, the vector will be reduced to its first `n` elements, and the rest will be destroyed.
    /// If `n` is greater than the current size, the vector will be expanded by appending default-constructed elements until the size reaches `n`.
    /// If `n` exceeds the capacity of the vector, this function will throw an exception.
    void resize(size_type n)
    {
        resize(n, {});
    }

    /// Resizes the vector to contain `n` elements.
    ///
    /// If `n` is smaller than the current size, the vector will be reduced to its first `n` elements, and the rest will be destroyed.
    /// If `n` is greater than the current size, the vector will be expanded by appending copies of `value` until the size reaches `n`.
    /// If `n` exceeds the capacity of the vector, this function will throw an exception.
    void resize(size_type n, const value_type& value)
    {
        if (size_ == n) {
            return;
        }
        POMDOG_ASSERT(size_ >= 0);
        POMDOG_ASSERT(n >= 0);
        if (n < size_) {
            while (n <= size_) {
                pop_back();
            }
        }
        else {
            while (n > size_) {
                push_back(value);
            }
        }
    }

private:
    /// Creates an iterator from a pointer. This is a helper function used internally for iterator operations.
    [[nodiscard]] static iterator make_iter(pointer p) noexcept
    {
        return iterator(p);
    }

    /// Grows the size of the vector to the specified new size `n`.
    /// If `n` exceeds the capacity of the vector, this function will throw an exception.
    void grow(size_type n)
    {
        if (n > capacity()) [[unlikely]] {
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__)
            std::__throw_out_of_range("FixedVector::grow");
#elif defined(_MSC_VER)
            std::_Xout_of_range("FixedVector::grow");
#else
            throw std::range_error("FixedVector::grow");
#endif
        }
        size_ = n;
    }

    /// Destroys a range of elements in the vector, starting from `start` and ending at `end`.
    static void destroy_range(pointer start, pointer end) noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        while (start != end) {
            --end;
            (*end).~value_type();
        }
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }

    POMDOG_MSVC_SUPPRESS_WARNING_PUSH
    POMDOG_MSVC_SUPPRESS_WARNING(4710)
    /// Destroys elements in the vector from `new_last` to the current end.
    void destruct_at_end(pointer new_last) noexcept(std::is_nothrow_destructible_v<value_type>)
    {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        pointer soon_to_be_end = std::addressof(data()[size_ - 1]) + 1;
        size_ = size_ - static_cast<size_type>(soon_to_be_end - new_last);
        destroy_range(new_last, soon_to_be_end);
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    POMDOG_MSVC_SUPPRESS_WARNING_POP
};

/// Returns true if the contents of the two vectors are equal, and false otherwise.
/// Two vectors are considered equal if they have the same size and their corresponding elements are equal.
template <typename Type, std::size_t Size>
inline bool
operator==(const FixedVector<Type, Size>& lhs, const FixedVector<Type, Size>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/// Returns true if the contents of the two vectors are not equal, and false otherwise.
/// Two vectors are considered not equal if they have different sizes or if any of their corresponding elements are not equal.
template <typename Type, std::size_t Size>
inline bool
operator!=(const FixedVector<Type, Size>& lhs, const FixedVector<Type, Size>& rhs)
{
    return !(lhs == rhs);
}

template <typename Type, std::size_t Size>
inline bool
operator<(const FixedVector<Type, Size>& lhs, const FixedVector<Type, Size>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type, std::size_t Size>
inline bool
operator>(const FixedVector<Type, Size>& lhs, const FixedVector<Type, Size>& rhs)
{
    return rhs < lhs;
}

template <typename Type, std::size_t Size>
inline bool
operator<=(const FixedVector<Type, Size>& lhs, const FixedVector<Type, Size>& rhs)
{
    return !(lhs > rhs);
}

template <typename Type, std::size_t Size>
inline bool
operator>=(const FixedVector<Type, Size>& lhs, const FixedVector<Type, Size>& rhs)
{
    return !(lhs < rhs);
}

} // namespace pomdog

namespace std {

template <typename Type, std::size_t Size>
inline constexpr typename std::enable_if_t<Size == 0 || std::is_swappable<Type>::value, void>
swap(pomdog::FixedVector<Type, Size>& x, pomdog::FixedVector<Type, Size>& y) noexcept(noexcept(x.swap(y)))
{
    x.swap(y);
}

template <typename Type, std::size_t Size>
struct tuple_size<pomdog::FixedVector<Type, Size>> {
    static inline constexpr std::size_t value = Size;
};

template <size_t I, typename Type, std::size_t Size>
struct tuple_element<I, pomdog::FixedVector<Type, Size>> {
    using type = Type;
};

template <size_t Index, typename Type, std::size_t Size>
inline constexpr Type&
get(pomdog::FixedVector<Type, Size>& v) noexcept
{
    static_assert(Index < Size, "Index out of bounds in std::get<> (pomdog::FixedVector)");
    return v[Index];
}

template <size_t Index, typename Type, std::size_t Size>
inline constexpr const Type&
get(const pomdog::FixedVector<Type, Size>& v) noexcept
{
    static_assert(Index < Size, "Index out of bounds in std::get<> (const pomdog::FixedVector)");
    return v[Index];
}

template <size_t Index, typename Type, std::size_t Size>
inline constexpr Type&&
get(pomdog::FixedVector<Type, Size>&& v) noexcept
{
    static_assert(Index < Size, "Index out of bounds in std::get<> (pomdog::FixedVector &&)");
    return std::move(v._elems[Index]);
}

template <size_t Index, typename Type, std::size_t Size>
inline constexpr const Type&&
get(const pomdog::FixedVector<Type, Size>&& v) noexcept
{
    static_assert(Index < Size, "Index out of bounds in std::get<> (const pomdog::FixedVector &&)");
    return std::move(v._elems[Index]);
}

} // namespace std
