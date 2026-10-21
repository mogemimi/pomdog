// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/composition_buffer.h"
#include "pomdog/experimental/text_input/text_input_event.h"
#include "pomdog/experimental/text_input/utf32_conversion.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput::detail {

void CompositionBuffer::clear() noexcept
{
    text_.clear();
    target_.clear();
    caret_ = 0;
}

void CompositionBuffer::replace(std::size_t first, std::size_t length, std::u32string_view text, std::span<const u8> target)
{
    first = std::min(first, text_.size());
    length = std::min(length, text_.size() - first);
    text_.replace(first, length, text);

    const auto begin = std::next(target_.begin(), static_cast<std::ptrdiff_t>(first));
    const auto erased = target_.erase(begin, std::next(begin, static_cast<std::ptrdiff_t>(length)));
    const auto inserted = target_.insert(erased, text.size(), u8(0));
    const auto marked = std::min(target.size(), text.size());
    std::copy_n(target.begin(), marked, inserted);

    caret_ = std::min(caret_, text_.size());
}

void CompositionBuffer::setTarget(std::size_t first, std::span<const u8> target)
{
    first = std::min(first, target_.size());
    const auto count = std::min(target.size(), target_.size() - first);
    std::copy_n(target.begin(), count, std::next(target_.begin(), static_cast<std::ptrdiff_t>(first)));
}

void CompositionBuffer::setCaret(std::size_t position) noexcept
{
    caret_ = std::min(position, text_.size());
}

std::size_t CompositionBuffer::getCaret() const noexcept
{
    return caret_;
}

std::size_t CompositionBuffer::getLength() const noexcept
{
    return text_.size();
}

void CompositionBuffer::fillEvent(TextInputEvent& event) const
{
    event.text.clear();
    appendUTF32AsUTF8(text_, event.text);
    event.caret = toUTF8Offset(text_, caret_);

    const auto first = std::find_if(target_.begin(), target_.end(), [](u8 mark) { return mark != 0; });
    const auto last = std::find(first, target_.end(), u8(0));
    const auto start = static_cast<std::size_t>(std::distance(target_.begin(), first));
    const auto end = static_cast<std::size_t>(std::distance(target_.begin(), last));
    event.targetStart = toUTF8Offset(text_, start);
    event.targetEnd = toUTF8Offset(text_, end);
}

} // namespace pomdog::textinput::detail
