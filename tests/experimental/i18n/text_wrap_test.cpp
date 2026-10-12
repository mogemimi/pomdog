// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/experimental/i18n/line_break_rules.h"
#include "pomdog/experimental/i18n/text_wrap.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::f32;
using pomdog::i32;
using pomdog::i18n::LineBreakLanguage;
using pomdog::i18n::wrapText;

namespace {

// NOTE: Every code point is 10 units wide, so expected break positions follow
// from character counts.
[[nodiscard]] f32
measureCodePoints(std::string_view text)
{
    const auto count = std::ranges::count_if(text, [](char c) {
        return (static_cast<unsigned char>(c) & 0xC0) != 0x80;
    });
    return 10.0f * static_cast<f32>(count);
}

[[nodiscard]] std::vector<std::string_view>
splitLines(std::string_view text)
{
    std::vector<std::string_view> lines;
    std::size_t start = 0;
    while (true) {
        const auto newline = text.find('\n', start);
        if (newline == std::string_view::npos) {
            lines.push_back(text.substr(start));
            break;
        }
        lines.push_back(text.substr(start, newline - start));
        start = newline + 1;
    }
    return lines;
}

[[nodiscard]] std::string
removeNewlines(std::string text)
{
    std::erase(text, '\n');
    return text;
}

[[nodiscard]] bool
isContinuationByte(char c)
{
    return (static_cast<unsigned char>(c) & 0xC0) == 0x80;
}

// NOTE: Decodes one complete UTF-8 sequence. The tests avoid utfcpp because
// its headers do not compile under the test warning flags.
[[nodiscard]] char32_t
decodeCodePoint(std::string_view sequence)
{
    const auto byteAt = [&](std::size_t index) -> char32_t {
        return static_cast<char32_t>(static_cast<unsigned char>(sequence.at(index)));
    };
    switch (sequence.size()) {
    case 1:
        return byteAt(0);
    case 2:
        return ((byteAt(0) & 0x1F) << 6) | (byteAt(1) & 0x3F);
    case 3:
        return ((byteAt(0) & 0x0F) << 12) | ((byteAt(1) & 0x3F) << 6) | (byteAt(2) & 0x3F);
    case 4:
        return ((byteAt(0) & 0x07) << 18) | ((byteAt(1) & 0x3F) << 12) | ((byteAt(2) & 0x3F) << 6) | (byteAt(3) & 0x3F);
    default:
        return U'\0';
    }
}

[[nodiscard]] char32_t
firstCodePoint(std::string_view line)
{
    std::size_t length = 1;
    while (length < line.size() && isContinuationByte(line.at(length))) {
        ++length;
    }
    return decodeCodePoint(line.substr(0, length));
}

[[nodiscard]] char32_t
lastCodePoint(std::string_view line)
{
    if (line.empty()) {
        return U'\0';
    }
    auto start = line.size() - 1;
    while (start > 0 && isContinuationByte(line.at(start))) {
        --start;
    }
    return decodeCodePoint(line.substr(start));
}

} // namespace

TEST_CASE("wrapText")
{
    SUBCASE("empty text is one empty line")
    {
        const auto result = wrapText(measureCodePoints, "", 100.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText.empty());
        CHECK(result.lineCount == 1);
    }
    SUBCASE("text that fits stays on one line")
    {
        const auto result = wrapText(measureCodePoints, "Hello", 100.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "Hello");
        CHECK(result.lineCount == 1);
    }
    SUBCASE("breaks at spaces")
    {
        auto result = wrapText(measureCodePoints, "Hello World", 60.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "Hello\nWorld");
        CHECK(result.lineCount == 2);

        result = wrapText(measureCodePoints, "aaa bbb ccc", 70.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "aaa bbb\nccc");
        CHECK(result.lineCount == 2);
    }
    SUBCASE("keeps paragraph breaks and normalizes them to LF")
    {
        const auto result = wrapText(measureCodePoints, "Line1\r\nLine2\rLine3\n", 100.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "Line1\nLine2\nLine3\n");
        CHECK(result.lineCount == 4);
    }
    SUBCASE("removes spaces at breaks and line ends but keeps indentation")
    {
        const auto result = wrapText(measureCodePoints, "  aa  bb  \n  cc  ", 40.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "  aa\nbb\n  cc");
        CHECK(result.lineCount == 3);
    }
    SUBCASE("measures each candidate line from its start")
    {
        bool measuredNewline = false;
        bool measuredLeadingSpace = false;
        const auto measure = [&](std::string_view text) -> f32 {
            measuredNewline = measuredNewline || (text.find('\n') != std::string_view::npos);
            measuredLeadingSpace = measuredLeadingSpace || (!text.empty() && text.front() == ' ');
            return measureCodePoints(text);
        };
        const auto result = wrapText(measure, "one two three four\nfive six seven", 90.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "one two\nthree\nfour\nfive six\nseven");
        CHECK_FALSE(measuredNewline);
        CHECK_FALSE(measuredLeadingSpace);
    }
    SUBCASE("keeps every line within maxWidth for space-separated text")
    {
        const std::string text = "The quick brown fox jumps over the lazy dog";
        for (i32 width = 50; width <= 200; width += 10) {
            const auto result = wrapText(measureCodePoints, text, static_cast<f32>(width), LineBreakLanguage::Default);
            std::string joined;
            for (const auto line : splitLines(result.wrappedText)) {
                CHECK(measureCodePoints(line) <= static_cast<f32>(width));
                CHECK_FALSE(line.empty());
                CHECK(line.front() != ' ');
                CHECK(line.back() != ' ');
                if (!joined.empty()) {
                    joined += ' ';
                }
                joined.append(line);
            }
            CHECK(joined == text);
        }
    }
    SUBCASE("breaks a word wider than maxWidth between characters")
    {
        const auto result = wrapText(measureCodePoints, "averylongidentifierwithoutspaces", 60.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "averyl\nongide\nntifie\nrwitho\nutspac\nes");
        CHECK(result.lineCount == 6);
    }
    SUBCASE("keeps closing punctuation with the word before it across a space")
    {
        const auto result = wrapText(measureCodePoints, "Il dit Bonjour !", 150.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "Il dit\nBonjour !");
    }
    SUBCASE("keeps French guillemets with the quoted words across spaces")
    {
        const auto result = wrapText(measureCodePoints, "Il dit « Bonjour »", 160.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "Il dit\n« Bonjour »");
    }
    SUBCASE("breaks before an opening quotation mark after a space")
    {
        const auto result = wrapText(measureCodePoints, "He said \"hi\"", 80.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "He said\n\"hi\"");
    }
    SUBCASE("keeps an opening bracket with the word after it across a space")
    {
        const auto result = wrapText(measureCodePoints, "aa ( bb", 40.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "aa\n( bb");
    }
    SUBCASE("does not break at a figure space")
    {
        // NOTE: U+2007 FIGURE SPACE aligns digits and keeps them together.
        const std::string text = "x 12\u200734";
        auto result = wrapText(measureCodePoints, text, 60.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "x\n12\u200734");

        result = wrapText(measureCodePoints, text, 70.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == text);
    }
    SUBCASE("breaks CJK text in Default only when a line overflows")
    {
        const auto result = wrapText(measureCodePoints, "これは ペンです", 50.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "これは\nペンです");
    }
    SUBCASE("breaks Japanese between characters")
    {
        const auto result = wrapText(measureCodePoints, "あいうえお", 25.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "あい\nうえ\nお");
        CHECK(result.lineCount == 3);
    }
    SUBCASE("does not start a Japanese line with closing punctuation")
    {
        const auto result = wrapText(measureCodePoints, "あいう、えお", 35.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "あい\nう、え\nお");
    }
    SUBCASE("does not start a Japanese line with small kana")
    {
        const auto result = wrapText(measureCodePoints, "あしょうか", 30.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "あしょ\nうか");
    }
    SUBCASE("does not start a Japanese line with an iteration mark")
    {
        const auto result = wrapText(measureCodePoints, "あ人々が", 25.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "あ\n人々\nが");
    }
    SUBCASE("does not start a Japanese line with halfwidth punctuation")
    {
        const auto result = wrapText(measureCodePoints, "ｱｲｳ\uFF61ｴｵ", 35.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "ｱｲ\nｳ\uFF61ｴ\nｵ");
    }
    SUBCASE("does not end a Japanese line with an opening bracket")
    {
        const auto result = wrapText(measureCodePoints, "あいう「えお」", 40.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "あいう\n「えお」");
    }
    SUBCASE("keeps numbers and Latin words whole in Japanese")
    {
        auto result = wrapText(measureCodePoints, "攻撃力が1000上がる", 60.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "攻撃力が\n1000上が\nる");

        result = wrapText(measureCodePoints, "これはPomdogです", 60.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "これは\nPomdog\nです");
    }
    SUBCASE("breaks Chinese between hanzi")
    {
        const auto result = wrapText(measureCodePoints, "你好，世界！欢迎", 30.0f, LineBreakLanguage::Chinese);
        CHECK(result.wrappedText == "你好，\n世界！\n欢迎");
    }
    SUBCASE("breaks Korean between syllables and at spaces")
    {
        const auto result = wrapText(measureCodePoints, "안녕하세요 세계", 40.0f, LineBreakLanguage::Korean);
        CHECK(result.wrappedText == "안녕하세\n요 세계");
    }
    SUBCASE("keeps Thai vowels and tone marks with their consonants")
    {
        const std::string text = "สวัสดีครับ ยินดีต้อนรับสู่เกมของเรา ขอให้สนุกกับการผจญภัย";
        for (i32 width = 30; width <= 150; width += 10) {
            const auto result = wrapText(measureCodePoints, text, static_cast<f32>(width), LineBreakLanguage::Thai);
            const auto lines = splitLines(result.wrappedText);
            CHECK(lines.size() == static_cast<std::size_t>(result.lineCount));
            for (const auto line : lines) {
                CHECK_FALSE(line.empty());
                CHECK_FALSE(pomdog::i18n::isLineStartProhibited(firstCodePoint(line)));
                CHECK_FALSE(pomdog::i18n::isLineEndProhibited(lastCodePoint(line)));
            }
        }
    }
    SUBCASE("does not start a Thai line with Mai Yamok")
    {
        const std::string text = "เล่นสนุกมากๆ";
        for (i32 width = 20; width <= 120; width += 10) {
            const auto result = wrapText(measureCodePoints, text, static_cast<f32>(width), LineBreakLanguage::Thai);
            for (const auto line : splitLines(result.wrappedText)) {
                CHECK(firstCodePoint(line) != U'\u0E46');
            }
        }
    }
    SUBCASE("keeps a Thai final consonant with its syllable")
    {
        const auto result = wrapText(measureCodePoints, "คุณดี", 40.0f, LineBreakLanguage::Thai);
        CHECK(result.wrappedText == "คุณ\nดี");
    }
    SUBCASE("keeps Thai combining marks with their consonant in a forced break")
    {
        // NOTE: Ko Kai (U+0E01) with Sara I (U+0E34) and Mai Ek (U+0E48) is
        // one character of three code points, wider than maxWidth.
        const std::string syllable = "\u0E01\u0E34\u0E48";
        auto result = wrapText(measureCodePoints, syllable, 10.0f, LineBreakLanguage::Thai);
        CHECK(result.wrappedText == syllable);
        CHECK(result.lineCount == 1);

        result = wrapText(measureCodePoints, syllable, 5.0f, LineBreakLanguage::Thai);
        CHECK(result.wrappedText == syllable);

        result = wrapText(measureCodePoints, syllable + syllable, 10.0f, LineBreakLanguage::Thai);
        CHECK(result.wrappedText == syllable + "\n" + syllable);
    }
    SUBCASE("breaks between emoji in Japanese")
    {
        const auto result = wrapText(measureCodePoints, "やったね😀😀😀", 50.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "やったね😀\n😀😀");
    }
    SUBCASE("keeps the regional indicators of a flag together")
    {
        // NOTE: The flags of Japan (U+1F1EF U+1F1F5) and the United States
        // (U+1F1FA U+1F1F8), each two code points wide.
        const std::string japan = "\U0001F1EF\U0001F1F5";
        const std::string unitedStates = "\U0001F1FA\U0001F1F8";
        auto result = wrapText(measureCodePoints, japan + unitedStates, 30.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == japan + "\n" + unitedStates);

        result = wrapText(measureCodePoints, japan + unitedStates, 30.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == japan + "\n" + unitedStates);

        // NOTE: A flag wider than maxWidth overflows rather than splitting.
        result = wrapText(measureCodePoints, japan, 10.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == japan);
        CHECK(result.lineCount == 1);

        // NOTE: Regional indicators pair up from the start of their run, so
        // the third one stands alone.
        result = wrapText(measureCodePoints, japan + "\U0001F1FA", 20.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == japan + "\n\U0001F1FA");

        result = wrapText(measureCodePoints, "やった" + japan, 40.0f, LineBreakLanguage::Japanese);
        CHECK(result.wrappedText == "やった\n" + japan);
    }
    SUBCASE("keeps emoji modifiers and joined sequences whole")
    {
        auto result = wrapText(measureCodePoints, "👍🏻👍🏻👍🏻", 35.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == "👍🏻\n👍🏻\n👍🏻");

        const std::string family = "👨\u200D👩\u200D👧";
        result = wrapText(measureCodePoints, family, 25.0f, LineBreakLanguage::Default);
        CHECK(result.wrappedText == family);
        CHECK(result.lineCount == 1);
    }
    SUBCASE("replaces invalid UTF-8 instead of terminating")
    {
        // NOTE: 0xDD is the MSVC debug-heap fill for freed memory, which a
        // dangling caller buffer would pass in.
        const std::string junk(120, '\xDD');
        const auto result = wrapText(measureCodePoints, junk, 340.0f, LineBreakLanguage::Default);
        CHECK(result.lineCount >= 1);
        CHECK(result.wrappedText.find('\xDD') == std::string::npos);
        CHECK(result.wrappedText.find("\xEF\xBF\xBD") != std::string::npos);
    }
    SUBCASE("replaces a truncated multi-byte sequence")
    {
        // NOTE: "あ" (E3 81 82) cut off after two bytes.
        const std::string truncated = "hello \xE3\x81";
        const auto wordBased = wrapText(measureCodePoints, truncated, 340.0f, LineBreakLanguage::Default);
        CHECK(wordBased.wrappedText == "hello \xEF\xBF\xBD");

        const auto cjk = wrapText(measureCodePoints, truncated, 340.0f, LineBreakLanguage::Japanese);
        CHECK(cjk.wrappedText == "hello \xEF\xBF\xBD");
    }
    SUBCASE("preserves the text apart from inserted breaks")
    {
        const std::string text = "ドワーフが穴を掘る。「宝石だ！」と叫んだ。";
        for (i32 width = 10; width <= 200; width += 10) {
            const auto result = wrapText(measureCodePoints, text, static_cast<f32>(width), LineBreakLanguage::Japanese);
            CHECK(removeNewlines(result.wrappedText) == text);
        }
    }
}
