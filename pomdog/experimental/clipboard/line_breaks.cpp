// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/line_breaks.h"

namespace pomdog::clipboard::detail {
namespace {

/// Calls appendLine for each line of text and appendBreak between lines.
template <typename AppendLine, typename AppendBreak>
void forEachLine(std::string_view text, AppendLine&& appendLine, AppendBreak&& appendBreak)
{
    std::size_t lineStart = 0;
    std::size_t i = 0;
    while (i < text.size()) {
        const auto c = text[i];
        if ((c != '\r') && (c != '\n')) {
            i += 1;
            continue;
        }
        appendLine(text.substr(lineStart, i - lineStart));
        appendBreak();
        if ((c == '\r') && (i + 1 < text.size()) && (text[i + 1] == '\n')) {
            i += 1;
        }
        i += 1;
        lineStart = i;
    }
    appendLine(text.substr(lineStart));
}

} // namespace

void appendWithLF(std::string_view text, std::string& result)
{
    forEachLine(
        text,
        [&](std::string_view line) { result.append(line); },
        [&] { result.push_back('\n'); });
}

void appendWithCRLF(std::string_view text, std::string& result)
{
    forEachLine(
        text,
        [&](std::string_view line) { result.append(line); },
        [&] { result.append("\r\n"); });
}

} // namespace pomdog::clipboard::detail
