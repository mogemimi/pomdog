// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/i18n/text_wrap.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/experimental/i18n/line_break_rules.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/utfcpp_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::i18n {
namespace {

constexpr char32_t zeroWidthJoiner = U'\u200D';

/// Reports whether codePoint is a pictographic emoji. CJK and Thai text may
/// break around it as around an ideograph.
///
/// The regional indicators of flags count too. A break between the two of a
/// flag splits a cluster, which the line wrapper checks first.
[[nodiscard]] bool
isEmojiCharacter(char32_t codePoint) noexcept
{
    // NOTE: Miscellaneous Symbols and Dingbats
    if (codePoint >= 0x2600 && codePoint <= 0x27BF) {
        return true;
    }
    // NOTE: Emoji blocks from Mahjong Tiles to Symbols and Pictographs Extended-A
    if (codePoint >= 0x1F000 && codePoint <= 0x1FAFF) {
        return true;
    }
    return false;
}

/// Reports whether CJK text may break before or after codePoint.
[[nodiscard]] bool
isIdeographic(char32_t codePoint) noexcept
{
    return isCJKCharacter(codePoint) || isEmojiCharacter(codePoint);
}

/// Reports whether codePoint stays with the text before it even across a
/// space, as "!" and the closing guillemet do in French "« Bonjour ! »".
[[nodiscard]] bool
isClosingPunctuation(char32_t codePoint) noexcept
{
    // NOTE: These correspond to the UAX #14 classes that do not start a line
    // even after spaces: closing brackets, exclamation and question marks,
    // and infix separators. Closing guillemets are added because French text
    // often puts a breakable space, not a no-break space, before them.
    switch (codePoint) {
    case U')':
    case U']':
    case U'}':
    case U',':
    case U'.':
    case U'!':
    case U'?':
    case U':':
    case U';':
    case U'、':
    case U'。':
    case U'，':
    case U'．':
    case U'：':
    case U'；':
    case U'！':
    case U'？':
    case U'）':
    case U'］':
    case U'｝':
    case U'」':
    case U'』':
    case U'】':
    case U'〉':
    case U'》':
    case U'〕':
    case U'〗':
    case U'〙':
    case U'〛':
    case U'\u301E': // Double prime quotation mark
    case U'\u301F': // Low double prime quotation mark
    case U'\uFF60': // Full-width right white parenthesis
    case U'\u2986': // Right white parenthesis
    case U'\uFF61': // Halfwidth ideographic full stop
    case U'\uFF63': // Halfwidth right corner bracket
    case U'\uFF64': // Halfwidth ideographic comma
    case U'\u203A': // Single right-pointing angle quotation mark
    case U'\u00BB': // Right-pointing double angle quotation mark
        return true;
    default:
        return false;
    }
}

/// Reports whether codePoint is a Thai consonant, from Ko Kai (U+0E01) to
/// Ho Nokhuk (U+0E2E).
[[nodiscard]] bool
isThaiConsonant(char32_t codePoint) noexcept
{
    return codePoint >= 0x0E01 && codePoint <= 0x0E2E;
}

/// Reports whether codePoint can be the second consonant of a Thai consonant
/// cluster, such as the "r" of "pr".
[[nodiscard]] bool
isThaiClusterConsonant(char32_t codePoint) noexcept
{
    // NOTE: Ro Rua (U+0E23), Lo Ling (U+0E25), and Wo Waen (U+0E27)
    return codePoint == 0x0E23 || codePoint == 0x0E25 || codePoint == 0x0E27;
}

/// Reports whether codePoint is a Thai vowel written above or below its
/// consonant.
[[nodiscard]] bool
isThaiAboveOrBelowVowel(char32_t codePoint) noexcept
{
    // NOTE: Mai Han-Akat (U+0E31), and Sara I (U+0E34) to Phinthu (U+0E3A)
    return codePoint == 0x0E31 || (codePoint >= 0x0E34 && codePoint <= 0x0E3A);
}

/// Reports whether Thai text may break between prev and next, where following
/// is the code point after next, or zero at the end of a paragraph.
///
/// Thai has no spaces between words, and finding syllables exactly needs a
/// dictionary, so these heuristics only keep likely syllables together.
/// Kinsoku rules, which already keep vowels and tone marks with their
/// consonant, are checked before this.
[[nodiscard]] bool
isThaiBreakAllowed(char32_t prev, char32_t next, char32_t following) noexcept
{
    // NOTE: A consonant right after an above or below vowel is likely the
    // final consonant of the same syllable, as in "คุณ" (ค + ุ + ณ).
    if (isThaiConsonant(next) && isThaiAboveOrBelowVowel(prev)) {
        return false;
    }
    // NOTE: Keep consonant clusters together, such as "ปร" in "ปรุง".
    if (isThaiClusterConsonant(next) && isThaiConsonant(prev)) {
        return false;
    }
    // NOTE: Keep two consonants together when a vowel follows the second.
    if (isThaiConsonant(prev) && isThaiConsonant(next)) {
        if (isThaiCombiningVowelOrToneMark(following) || isThaiLeadingVowel(following)) {
            return false;
        }
    }
    return true;
}

/// Returns how many leading indices in [0, count) satisfy fits. fits must hold
/// for some prefix of the indices and fail for the rest. Probes start near
/// zero and double their distance, so the number and length of measurements
/// follow the answer rather than count.
template <typename Fits>
[[nodiscard]] std::size_t
countLeadingFits(std::size_t count, const Fits& fits)
{
    std::size_t low = 0;
    std::size_t high = count;
    std::size_t step = 1;
    while (low < high) {
        const auto probe = std::min(low + step - 1, high - 1);
        if (!fits(probe)) {
            high = probe;
            break;
        }
        low = probe + 1;
        step *= 2;
    }
    while (low < high) {
        const auto middle = low + (high - low) / 2;
        if (fits(middle)) {
            low = middle + 1;
        }
        else {
            high = middle;
        }
    }
    return low;
}

/// A position before codePoints[position] where a line may break. The line
/// ends at contentEnd, which excludes the spaces before the break.
struct BreakCandidate final {
    std::size_t position = 0;
    std::size_t contentEnd = 0;
};

/// Breaks valid UTF-8 text into lines. Positions are code point indices.
class LineWrapper final {
private:
    const TextMeasureFunc& measureFunc_;
    const std::string& source_;
    std::vector<char32_t> codePoints_;
    // NOTE: offsets_[i] is the byte offset of codePoints_[i]; the last entry
    // is the size of source_.
    std::vector<std::size_t> offsets_;
    std::vector<BreakCandidate> candidates_;
    WrappedTextResult result_;
    f32 maxWidth_ = 0.0f;
    LineBreakLanguage language_;

public:
    LineWrapper(
        const TextMeasureFunc& measureFunc,
        const std::string& source,
        f32 maxWidth,
        LineBreakLanguage language)
        : measureFunc_(measureFunc)
        , source_(source)
        , maxWidth_(maxWidth)
        , language_(language)
    {
        codePoints_.reserve(source_.size());
        offsets_.reserve(source_.size() + 1);

        auto iter = source_.cbegin();
        const auto end = source_.cend();
        while (iter != end) {
            offsets_.push_back(static_cast<std::size_t>(std::distance(source_.cbegin(), iter)));
            codePoints_.push_back(utf8::next(iter, end));
        }
        offsets_.push_back(source_.size());
    }

    [[nodiscard]] WrappedTextResult
    wrap()
    {
        result_.wrappedText.reserve(source_.size());

        const auto count = codePoints_.size();
        std::size_t paragraphBegin = 0;
        for (std::size_t index = 0; index < count; ++index) {
            const char32_t codePoint = codePoints_[index];
            if (codePoint == U'\n' || codePoint == U'\r') {
                wrapParagraph(paragraphBegin, index);
                appendNewline();
                if (codePoint == U'\r' && index + 1 < count && codePoints_[index + 1] == U'\n') {
                    ++index;
                }
                paragraphBegin = index + 1;
            }
        }
        wrapParagraph(paragraphBegin, count);
        return std::move(result_);
    }

private:
    [[nodiscard]] bool
    fits(std::size_t begin, std::size_t end) const
    {
        const auto offset = offsets_[begin];
        return measureFunc_(std::string_view{source_}.substr(offset, offsets_[end] - offset)) <= maxWidth_;
    }

    void appendLine(std::size_t begin, std::size_t end)
    {
        const auto offset = offsets_[begin];
        result_.wrappedText.append(source_, offset, offsets_[end] - offset);
    }

    void appendNewline()
    {
        result_.wrappedText += '\n';
        ++result_.lineCount;
    }

    /// Reports whether breaking before index would split a character from the
    /// combining marks or emoji joined to it, or a flag between its regional
    /// indicators.
    [[nodiscard]] bool
    splitsCluster(std::size_t index, std::size_t paragraphEnd) const noexcept
    {
        if (index >= paragraphEnd) {
            return false;
        }
        if (isClusterExtender(codePoints_[index]) || codePoints_[index - 1] == zeroWidthJoiner) {
            return true;
        }
        // NOTE: Regional indicators pair up from the start of their run, so a
        // break after an odd number of them splits a flag.
        return isRegionalIndicator(codePoints_[index]) && ((countRegionalIndicatorsBefore(index) % 2) == 1);
    }

    /// Returns how many regional indicators come right before index.
    [[nodiscard]] std::size_t
    countRegionalIndicatorsBefore(std::size_t index) const noexcept
    {
        std::size_t count = 0;
        while ((index > 0) && isRegionalIndicator(codePoints_[index - 1])) {
            --index;
            ++count;
        }
        return count;
    }

    [[nodiscard]] bool
    satisfiesKinsoku(std::size_t index, std::size_t paragraphEnd) const noexcept
    {
        if (index < paragraphEnd && isLineStartProhibited(codePoints_[index])) {
            return false;
        }
        return !isLineEndProhibited(codePoints_[index - 1]);
    }

    /// Reports whether the rules of language_ allow a break before index.
    /// lastNonSpace is the last non-space code point before index, or zero.
    [[nodiscard]] bool
    canBreakBefore(std::size_t index, std::size_t paragraphEnd, char32_t lastNonSpace) const noexcept
    {
        const char32_t prev = codePoints_[index - 1];
        const char32_t next = codePoints_[index];

        // NOTE: Break after a run of spaces, not inside or before it.
        if (isBreakableWhitespace(next) || splitsCluster(index, paragraphEnd)) {
            return false;
        }
        if (isBreakableWhitespace(prev)) {
            if (isClosingPunctuation(next)) {
                return false;
            }
            return !isLineEndProhibited(lastNonSpace);
        }
        if (isLineStartProhibited(next) || isLineEndProhibited(prev)) {
            return false;
        }

        switch (language_) {
        case LineBreakLanguage::Default:
            return false;
        case LineBreakLanguage::Japanese:
        case LineBreakLanguage::Chinese:
        case LineBreakLanguage::Korean:
            return isIdeographic(prev) || isIdeographic(next);
        case LineBreakLanguage::Thai:
            if (isThaiCharacter(prev) || isThaiCharacter(next)) {
                const char32_t following = (index + 1 < paragraphEnd) ? codePoints_[index + 1] : U'\0';
                return isThaiBreakAllowed(prev, next, following);
            }
            return isIdeographic(prev) || isIdeographic(next);
        }
        return false;
    }

    void collectBreakCandidates(std::size_t paragraphBegin, std::size_t paragraphEnd)
    {
        candidates_.clear();
        char32_t lastNonSpace = U'\0';
        std::size_t contentEnd = paragraphBegin;
        for (auto index = paragraphBegin + 1; index < paragraphEnd; ++index) {
            const char32_t prev = codePoints_[index - 1];
            if (!isBreakableWhitespace(prev)) {
                lastNonSpace = prev;
                contentEnd = index;
            }
            if (canBreakBefore(index, paragraphEnd, lastNonSpace)) {
                candidates_.push_back(BreakCandidate{.position = index, .contentEnd = contentEnd});
            }
        }
    }

    /// Returns where to break a line starting at lineBegin when no candidate
    /// fits: the widest fitting position in (lineBegin, limit], moved back to
    /// avoid kinsoku violations and split clusters where possible.
    [[nodiscard]] std::size_t
    findForcedBreak(std::size_t lineBegin, std::size_t limit, std::size_t paragraphEnd) const
    {
        POMDOG_ASSERT(lineBegin < limit);
        const auto fitCount = countLeadingFits(limit - lineBegin, [&](std::size_t k) {
            return fits(lineBegin, lineBegin + k + 1);
        });

        // NOTE: Keep at least one character per line, even when it is wider
        // than maxWidth, so that wrapping always makes progress.
        const auto widest = lineBegin + std::max<std::size_t>(fitCount, 1);

        for (auto index = widest; index > lineBegin; --index) {
            if (!splitsCluster(index, paragraphEnd) && satisfiesKinsoku(index, paragraphEnd)) {
                return index;
            }
        }
        for (auto index = widest; index > lineBegin; --index) {
            if (!splitsCluster(index, paragraphEnd)) {
                return index;
            }
        }
        // NOTE: A cluster wider than maxWidth overflows rather than splitting.
        auto index = widest + 1;
        while (splitsCluster(index, paragraphEnd)) {
            ++index;
        }
        return index;
    }

    void wrapParagraph(std::size_t paragraphBegin, std::size_t paragraphEnd)
    {
        // NOTE: Spaces at the end of a line are removed, so they never count
        // toward its width.
        auto contentEnd = paragraphEnd;
        while (contentEnd > paragraphBegin && isBreakableWhitespace(codePoints_[contentEnd - 1])) {
            --contentEnd;
        }

        collectBreakCandidates(paragraphBegin, paragraphEnd);

        auto lineBegin = paragraphBegin;
        std::size_t firstCandidate = 0;
        while (lineBegin < contentEnd) {
            // NOTE: Skip candidates that would leave the line empty, such as
            // one right after the leading spaces of a paragraph.
            while (firstCandidate < candidates_.size() && candidates_[firstCandidate].contentEnd <= lineBegin) {
                ++firstCandidate;
            }

            // NOTE: The end of the paragraph serves as the last candidate, so
            // the remaining text stays on one line when it fits.
            const auto candidateCount = candidates_.size() - firstCandidate;
            const auto candidateContentEnd = [&](std::size_t k) -> std::size_t {
                if (k < candidateCount) {
                    return candidates_[firstCandidate + k].contentEnd;
                }
                return contentEnd;
            };
            const auto fitCount = countLeadingFits(candidateCount + 1, [&](std::size_t k) {
                return fits(lineBegin, candidateContentEnd(k));
            });

            if (fitCount > candidateCount) {
                appendLine(lineBegin, contentEnd);
                return;
            }
            if (fitCount > 0) {
                const auto& candidate = candidates_[firstCandidate + fitCount - 1];
                appendLine(lineBegin, candidate.contentEnd);
                appendNewline();
                lineBegin = candidate.position;
                firstCandidate += fitCount;
                continue;
            }

            // NOTE: Not even the first word fits, so break inside it.
            const auto breakIndex = findForcedBreak(lineBegin, candidateContentEnd(0), paragraphEnd);
            if (breakIndex >= contentEnd) {
                appendLine(lineBegin, contentEnd);
                return;
            }
            auto lineEnd = breakIndex;
            while (lineEnd > lineBegin && isBreakableWhitespace(codePoints_[lineEnd - 1])) {
                --lineEnd;
            }
            appendLine(lineBegin, lineEnd);
            appendNewline();
            lineBegin = breakIndex;
            while (lineBegin < paragraphEnd && isBreakableWhitespace(codePoints_[lineBegin])) {
                ++lineBegin;
            }
        }
    }
};

} // namespace

WrappedTextResult
wrapText(
    const TextMeasureFunc& measureFunc,
    std::string_view text,
    f32 maxWidth,
    LineBreakLanguage language) noexcept
{
    POMDOG_ASSERT(measureFunc != nullptr);
    POMDOG_ASSERT(maxWidth > 0.0f);

    // NOTE: utfcpp throws on invalid input, which would terminate this
    // noexcept function. Replacing invalid sequences first keeps decoding in
    // LineWrapper from throwing; valid text costs only the validation scan.
    std::string source{text};
    if (!utf8::is_valid(source.cbegin(), source.cend())) {
        std::string sanitized;
        utf8::replace_invalid(source.cbegin(), source.cend(), std::back_inserter(sanitized));
        source = std::move(sanitized);
    }

    LineWrapper wrapper{measureFunc, source, maxWidth, language};
    return wrapper.wrap();
}

} // namespace pomdog::i18n
