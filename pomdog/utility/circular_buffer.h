// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/aligned_byte_array.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename T>
class CircularBuffer;

} // namespace pomdog

namespace pomdog::detail {

/// Represents an iterator for the CircularBuffer class.
template <typename CircularBufferPointer, typename T>
class CircularBufferIterator final {
public:
    CircularBufferPointer buffer_ = nullptr;
    std::size_t index_ = 0;

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

public:
    CircularBufferIterator(CircularBufferPointer buffer, std::size_t index) noexcept
        : buffer_(buffer)
        , index_(index)
    {
    }

    reference operator*() const noexcept
    {
        return (*buffer_)[index_];
    }

    pointer operator->() noexcept
    {
        return &(*buffer_)[index_];
    }

    CircularBufferIterator& operator++() noexcept
    {
        index_++;
        return *this;
    }

    CircularBufferIterator operator++(int) noexcept
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    friend bool operator==(const CircularBufferIterator& a, const CircularBufferIterator& b) noexcept
    {
        POMDOG_ASSERT(a.buffer_ == b.buffer_);
        return a.index_ == b.index_;
    }

    friend bool operator!=(const CircularBufferIterator& a, const CircularBufferIterator& b) noexcept
    {
        POMDOG_ASSERT(a.buffer_ == b.buffer_);
        return a.index_ != b.index_;
    }
};

} // namespace pomdog::detail

namespace pomdog {

/// Represents a circular buffer (also known as a ring buffer) that can hold a fixed number of elements.
/// When the buffer is full and a new element is added, the oldest element in the buffer will be overwritten.
template <typename T>
class CircularBuffer final {
public:
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = detail::CircularBufferIterator<CircularBuffer<T>*, T>;
    using const_iterator = detail::CircularBufferIterator<const CircularBuffer<T>*, const T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using storage_type = detail::AlignedByteArray<alignof(value_type), sizeof(value_type)>;
    static_assert(std::is_standard_layout<storage_type>::value);

private:
    std::unique_ptr<storage_type[]> buffer_;
    size_type capacity_ = 0;
    size_type head_ = 0;
    size_type tail_ = 0;
    bool full_ = false;

public:
    /// Default constructor. Initializes an empty circular buffer with a capacity of 0.
    CircularBuffer() = default;

    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;

    /// Move constructor. Moves the contents of `rhs` to this circular buffer.
    /// After the move, `rhs` will be left in an empty state.
    CircularBuffer(CircularBuffer&& rhs) noexcept
        : buffer_(std::move(rhs.buffer_))
        , capacity_(rhs.capacity_)
        , head_(rhs.head_)
        , tail_(rhs.tail_)
        , full_(rhs.full_)
    {
        rhs.capacity_ = 0;
        rhs.head_ = 0;
        rhs.tail_ = 0;
        rhs.full_ = false;
    }

    /// Move assignment operator. Moves the contents of `rhs` to this circular buffer.
    /// After the move, `rhs` will be left in an empty state.
    CircularBuffer& operator=(CircularBuffer&& rhs) noexcept
    {
        if (this != &rhs) {
            clear();
            buffer_ = std::move(rhs.buffer_);
            capacity_ = rhs.capacity_;
            head_ = rhs.head_;
            tail_ = rhs.tail_;
            full_ = rhs.full_;
            rhs.capacity_ = 0;
            rhs.head_ = 0;
            rhs.tail_ = 0;
            rhs.full_ = false;
        }
        return *this;
    }

    /// Destructor. Destroys all elements in the circular buffer and releases resources.
    ~CircularBuffer()
    {
        clear();
    }

    /// Constructs a circular buffer with the specified capacity.
    /// The buffer will be able to hold up to `bufferSize` elements.
    explicit CircularBuffer(size_type bufferSize);

    /// Returns an iterator to the beginning of the circular buffer.
    /// The iterator points to the first element in the buffer, or to the end if the buffer is empty.
    iterator begin() noexcept;

    /// Returns a const iterator to the beginning of the circular buffer.
    /// The iterator points to the first element in the buffer, or to the end if the buffer is empty.
    const_iterator begin() const noexcept;

    /// Returns a const iterator to the beginning of the circular buffer.
    /// The iterator points to the first element in the buffer, or to the end if the buffer is empty.
    const_iterator cbegin() const noexcept;

    /// Returns an iterator to the end of the circular buffer.
    /// The iterator points to the position just past the last element in the buffer.
    iterator end() noexcept;

    /// Returns a const iterator to the end of the circular buffer.
    /// The iterator points to the position just past the last element in the buffer.
    const_iterator end() const noexcept;

    /// Returns a const iterator to the end of the circular buffer.
    /// The iterator points to the position just past the last element in the buffer.
    const_iterator cend() const noexcept;

    /// Returns the number of elements currently stored in the circular buffer.
    [[nodiscard]] size_type size() const noexcept;

    /// Returns the maximum number of elements that the circular buffer can hold, which is equal to the capacity.
    [[nodiscard]] size_type max_size() const noexcept;

    /// Returns the maximum number of elements that the circular buffer can hold, which is equal to the capacity.
    [[nodiscard]] size_type capacity() const noexcept;

    /// Returns true if the circular buffer is empty (i.e., contains no elements), and false otherwise.
    [[nodiscard]] bool empty() const noexcept;

    /// Returns true if the circular buffer is full (i.e., contains the maximum number of elements), and false otherwise.
    [[nodiscard]] bool full() const noexcept;

    /// Returns a reference to the element at the specified index.
    /// The index is zero-based, and the element at index 0 is the oldest element in the buffer (the front),
    /// while the element at index `size() - 1` is the newest element in the buffer (the back).
    [[nodiscard]] reference operator[](size_type index) noexcept;

    /// Returns a const reference to the element at the specified index.
    /// The index is zero-based, and the element at index 0 is the oldest element in the buffer (the front),
    /// while the element at index `size() - 1` is the newest element in the buffer (the back).
    [[nodiscard]] const_reference operator[](size_type index) const noexcept;

    /// Returns a reference to the element at the specified index.
    /// The index is zero-based, and the element at index 0 is the oldest element in the buffer (the front),
    /// while the element at index `size() - 1` is the newest element in the buffer (the back).
    [[nodiscard]] reference at(size_type index) noexcept;

    /// Returns a const reference to the element at the specified index.
    /// The index is zero-based, and the element at index 0 is the oldest element in the buffer (the front),
    /// while the element at index `size() - 1` is the newest element in the buffer (the back).
    [[nodiscard]] const_reference at(size_type index) const noexcept;

    /// Returns a reference to the first element in the circular buffer.
    /// The first element is the oldest element in the buffer (the front).
    [[nodiscard]] reference front() noexcept;

    /// Returns a const reference to the first element in the circular buffer.
    /// The first element is the oldest element in the buffer (the front).
    [[nodiscard]] const_reference front() const noexcept;

    /// Returns a reference to the last element in the circular buffer.
    /// The last element is the newest element in the buffer (the back).
    [[nodiscard]] reference back() noexcept;

    /// Returns a const reference to the last element in the circular buffer.
    /// The last element is the newest element in the buffer (the back).
    [[nodiscard]] const_reference back() const noexcept;

    /// Adds a new element to the front of the circular buffer.
    /// If the buffer is full, the oldest element at the back will be overwritten by the new element.
    void push_front(value_type&& item) noexcept(std::is_nothrow_move_constructible_v<value_type>);

    /// Adds a new element to the back of the circular buffer.
    /// If the buffer is full, the oldest element at the front will be overwritten by the new element.
    void push_back(value_type&& item) noexcept(std::is_nothrow_move_constructible_v<value_type>);

    /// Removes the first element from the circular buffer.
    /// After this operation, the size of the buffer will be reduced by one.
    void pop_front() noexcept(std::is_nothrow_destructible_v<value_type>);

    /// Removes the last element from the circular buffer.
    /// After this operation, the size of the buffer will be reduced by one.
    void pop_back() noexcept(std::is_nothrow_destructible_v<value_type>);

    /// Clears the circular buffer, removing all elements and resetting its state to empty.
    void clear();

    /// Swaps the contents of this circular buffer with another circular buffer `rhs`.
    /// This operation is noexcept if the element type is nothrow swappable.
    void swap(CircularBuffer& rhs) noexcept(std::is_nothrow_swappable_v<value_type>);
};

template <typename T>
inline CircularBuffer<T>::CircularBuffer(std::size_t bufferSize)
{
    buffer_ = std::unique_ptr<storage_type[]>(new storage_type[bufferSize]);
    capacity_ = bufferSize;
}

template <typename T>
inline typename CircularBuffer<T>::iterator CircularBuffer<T>::begin() noexcept
{
    return iterator(this, 0);
}

template <typename T>
inline typename CircularBuffer<T>::const_iterator CircularBuffer<T>::begin() const noexcept
{
    return const_iterator(this, 0);
}

template <typename T>
inline typename CircularBuffer<T>::const_iterator CircularBuffer<T>::cbegin() const noexcept
{
    return const_iterator(this, 0);
}

template <typename T>
inline typename CircularBuffer<T>::iterator CircularBuffer<T>::end() noexcept
{
    return iterator(this, size());
}

template <typename T>
inline typename CircularBuffer<T>::const_iterator CircularBuffer<T>::end() const noexcept
{
    return const_iterator(this, size());
}

template <typename T>
inline typename CircularBuffer<T>::const_iterator CircularBuffer<T>::cend() const noexcept
{
    return const_iterator(this, size());
}

template <typename T>
inline typename CircularBuffer<T>::reference CircularBuffer<T>::operator[](size_type index) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    return reinterpret_cast<T*>(buffer_.get())[(tail_ + index) % capacity_];
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

template <typename T>
inline typename CircularBuffer<T>::const_reference CircularBuffer<T>::operator[](size_type index) const noexcept
{
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    return reinterpret_cast<T*>(buffer_.get())[(tail_ + index) % capacity_];
}

template <typename T>
inline typename CircularBuffer<T>::reference CircularBuffer<T>::at(size_type index) noexcept
{
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    return reinterpret_cast<T*>(buffer_.get())[(tail_ + index) % capacity_];
}

template <typename T>
inline typename CircularBuffer<T>::const_reference CircularBuffer<T>::at(size_type index) const noexcept
{
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    return reinterpret_cast<T*>(buffer_.get())[(tail_ + index) % capacity_];
}

template <typename T>
inline typename CircularBuffer<T>::size_type CircularBuffer<T>::size() const noexcept
{
    if (full_) {
        return capacity_;
    }
    if (head_ >= tail_) {
        return head_ - tail_;
    }
    return capacity_ + head_ - tail_;
}

template <typename T>
inline typename CircularBuffer<T>::size_type CircularBuffer<T>::max_size() const noexcept
{
    return capacity_;
}

template <typename T>
inline typename CircularBuffer<T>::size_type CircularBuffer<T>::capacity() const noexcept
{
    return capacity_;
}

template <typename T>
inline bool CircularBuffer<T>::empty() const noexcept
{
    return !full_ && (head_ == tail_);
}

template <typename T>
inline bool CircularBuffer<T>::full() const noexcept
{
    return full_;
}

template <typename T>
inline typename CircularBuffer<T>::reference CircularBuffer<T>::front() noexcept
{
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    return reinterpret_cast<T*>(buffer_.get())[tail_];
}

template <typename T>
inline typename CircularBuffer<T>::const_reference CircularBuffer<T>::front() const noexcept
{
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    return reinterpret_cast<T*>(buffer_.get())[tail_];
}

template <typename T>
inline typename CircularBuffer<T>::reference CircularBuffer<T>::back() noexcept
{
    static_assert(std::is_unsigned_v<decltype(head_)>, "head_ must be >= 0");
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    POMDOG_ASSERT(head_ < capacity_);
    return reinterpret_cast<T*>(buffer_.get())[((head_ + capacity_) - 1) % capacity_];
}

template <typename T>
inline typename CircularBuffer<T>::const_reference CircularBuffer<T>::back() const noexcept
{
    static_assert(std::is_unsigned_v<decltype(head_)>, "head_ must be >= 0");
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    POMDOG_ASSERT(head_ < capacity_);
    return reinterpret_cast<T*>(buffer_.get())[((head_ + capacity_) - 1) % capacity_];
}

template <typename T>
inline void CircularBuffer<T>::push_front(value_type&& item) noexcept(std::is_nothrow_move_constructible_v<value_type>)
{
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);

    if (full_) {
        pop_back();
    }

    tail_ = ((tail_ + capacity_) - 1) % capacity_;
    new (buffer_.get() + tail_) value_type(std::forward<value_type>(item));
    full_ = (head_ == tail_);
}

template <typename T>
inline void CircularBuffer<T>::push_back(value_type&& item) noexcept(std::is_nothrow_move_constructible_v<value_type>)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);

    if (full_) {
        pop_front();
    }

    new (buffer_.get() + head_) value_type(std::forward<value_type>(item));
    head_ = (head_ + 1) % capacity_;
    full_ = (head_ == tail_);
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

template <typename T>
inline void CircularBuffer<T>::pop_front() noexcept(std::is_nothrow_destructible_v<value_type>)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    POMDOG_ASSERT(!empty());

    auto& value = reinterpret_cast<T*>(buffer_.get())[tail_];
    value.~value_type();
    full_ = false;
    tail_ = (tail_ + 1) % capacity_;
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

template <typename T>
inline void CircularBuffer<T>::pop_back() noexcept(std::is_nothrow_destructible_v<value_type>)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    POMDOG_ASSERT(buffer_ != nullptr);
    POMDOG_ASSERT(capacity_ > 0);
    POMDOG_ASSERT(head_ >= 0);
    POMDOG_ASSERT(head_ < capacity_);
    POMDOG_ASSERT(!empty());

    const auto index = ((head_ + capacity_) - 1) % capacity_;
    auto& value = reinterpret_cast<T*>(buffer_.get())[index];
    value.~value_type();
    full_ = false;
    head_ = index;
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

template <typename T>
inline void CircularBuffer<T>::clear()
{
    while (!empty()) {
        pop_front();
    }
}

template <typename T>
inline void CircularBuffer<T>::swap(CircularBuffer& rhs) noexcept(std::is_nothrow_swappable_v<value_type>)
{
    std::swap(buffer_, rhs.buffer_);
    std::swap(capacity_, rhs.capacity_);
    std::swap(head_, rhs.head_);
    std::swap(tail_, rhs.tail_);
    std::swap(full_, rhs.full_);
}

} // namespace pomdog
