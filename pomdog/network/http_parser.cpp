// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_parser.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using TextIterator = std::string_view::const_iterator;

/// Returns bytes as text. HTTP headers are ASCII.
[[nodiscard]] std::string_view
asText(const std::vector<u8>& bytes) noexcept
{
    return std::string_view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};
}

/// Reads the characters before the first one that isSeparator accepts, and
/// moves it to that character.
template <class Separator>
[[nodiscard]] std::expected<std::string_view, std::unique_ptr<Error>>
getWord(TextIterator& it, TextIterator end, Separator isSeparator)
{
    const auto begin = it;
    std::size_t count = 0;

    for (; it != end; ++it) {
        const auto c = *it;
        if (isSeparator(c)) {
            break;
        }
        if (c <= 31) {
            return std::unexpected(errors::make("Invalid HTTP Response"));
        }
        ++count;
    }
    if (count == 0) {
        return std::string_view{};
    }
    return std::string_view{&(*begin), count};
}

[[nodiscard]] TextIterator
skip(TextIterator begin, TextIterator end, char character)
{
    auto it = begin;

    // NOTE: Skip white-spaces
    for (; it != end; ++it) {
        if (*it != character) {
            break;
        }
    }
    return it;
}

char toLower(char c) noexcept
{
    static_assert('A' < 'Z');
    static_assert('A' < 'a');
    if ((c >= 'A') && (c <= 'Z')) {
        return (c - 'A') + 'a';
    }
    return c;
}

bool isDigit(char c) noexcept
{
    static_assert('0' < '9');
    return (c >= '0') && (c <= '9');
}

bool compareCaseInsensitive(std::string_view a, std::string_view b) noexcept
{
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b),
        [](char x, char y) {
            return (toLower(x) == toLower(y));
        });
}

bool readChar(TextIterator& it, TextIterator end, char character)
{
    if ((it != end) && (*it == character)) {
        ++it;
        return true;
    }
    return false;
}

bool isSpace(char c) noexcept
{
    return (c == ' ');
}

bool isLinebreak(char c) noexcept
{
    return (c == '\n') || (c == '\r');
}

[[nodiscard]] std::expected<std::vector<std::pair<std::string, std::string>>, std::unique_ptr<Error>>
parseFields(TextIterator& iter, TextIterator end)
{
    std::vector<std::pair<std::string, std::string>> fields;

    while (iter != end) {
        auto line = getWord(iter, end, isLinebreak);
        if (!line) {
            return std::unexpected(std::move(line.error()));
        }

        auto pos = line->find(": ");
        if (pos == std::string_view::npos) {
            if (line->empty()) {
                // NOTE: end of field
                break;
            }
            else {
                return std::unexpected(errors::make("Invalid HTTP Response field"));
            }
        }

        auto key = line->substr(0, pos);
        auto value = line->substr(pos + 2);
        fields.emplace_back(key, value);

        iter = skip(iter, end, '\r');
        if (!readChar(iter, end, '\n')) {
            return std::unexpected(errors::make("Invalid HTTP Response field"));
        }
    }

    return fields;
}

/// Returns the value of the field named key. Field names are case-insensitive
/// (RFC 9110, section 5.1), and many servers send them in lowercase.
template <typename T>
std::optional<std::string_view> findField(T& fields, std::string_view key)
{
    auto iter = std::find_if(std::begin(fields), std::end(fields),
        [&](const std::pair<std::string, std::string>& field) -> bool {
            return compareCaseInsensitive(field.first, key);
        });

    if (iter != std::end(fields)) {
        return iter->second;
    }
    return std::nullopt;
}

[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
parseResponse(HTTPResponse& response, std::vector<u8>& rawData)
{
    const auto text = asText(rawData);
    const auto end = text.end();
    auto iter = text.begin();

    auto word = getWord(iter, end, isSpace);
    if (!word) {
        return std::unexpected(std::move(word.error()));
    }

    if ((word->size() >= 8) &&
        ((*word)[6] == '.') &&
        (compareCaseInsensitive(word->substr(0, 5), "HTTP/")) &&
        isDigit((*word)[5]) &&
        isDigit((*word)[7])) {
        response.protocol = *word;
    }
    else {
        return std::unexpected(errors::make("Invalid HTTP Response protocol"));
    }

    iter = skip(iter, end, ' ');
    auto status = getWord(iter, end, isLinebreak);
    if (!status) {
        return std::unexpected(std::move(status.error()));
    }

    if ((status->size() >= 4) && ((*status)[3] == ' ')) {
        std::stringstream ss;
        ss << (*status)[0] << (*status)[1] << (*status)[2];
        if (!(ss >> response.statusCode)) {
            return std::unexpected(errors::make("Invalid HTTP Response status"));
        }
        response.status = *status;
    }
    else {
        return std::unexpected(errors::make("Invalid HTTP Response status"));
    }

    iter = skip(iter, end, '\r');
    if (!readChar(iter, end, '\n')) {
        return std::unexpected(errors::make("Invalid HTTP Response"));
    }

    auto fields = parseFields(iter, end);
    if (!fields) {
        return std::unexpected(std::move(fields.error()));
    }
    response.headers = std::move(*fields);

    iter = skip(iter, end, '\r');
    if (!readChar(iter, end, '\n')) {
        return std::unexpected(errors::make("Invalid HTTP Response"));
    }

    if (auto field = findField(response.headers, "Content-Length"); field.has_value()) {
        std::stringstream ss;
        ss << *field;
        size_t length;
        if (!(ss >> length)) {
            return std::unexpected(errors::make("Invalid HTTP Response 'Content-Length'"));
        }
        response.contentLength = length;
    }

    if (compareCaseInsensitive(response.protocol, "HTTP/1.1")) {
        if (auto field = findField(response.headers, "Transfer-Encoding"); field.has_value()) {
            response.chunkedTransferEncoding = (field->find("chunked") != std::string::npos);
        }
    }

    // NOTE: Remove the header, and keep the body.
    rawData.erase(std::begin(rawData), std::next(std::begin(rawData), std::distance(text.begin(), iter)));

    return {};
}

bool isResponseHeaderReadable(const std::vector<u8>& rawData)
{
    return asText(rawData).find("\r\n\r\n") != std::string_view::npos;
}

} // namespace

std::expected<HTTPParseResult, std::unique_ptr<Error>>
HTTPParser::parse(std::span<const u8> data)
{
    bool hasEOFChunk = false;

    rawData_.insert(std::end(rawData_), std::begin(data), std::end(data));

    if (!response_.has_value()) {
        if (!isResponseHeaderReadable(rawData_)) {
            return HTTPParseResult::WouldBlock;
        }

        auto& response = response_.emplace();
        if (auto result = parseResponse(response, rawData_); !result) {
            return std::unexpected(std::move(result.error()));
        }

        if (response.chunkedTransferEncoding) {
            requiredContentLength_ = 0;
        }
        else {
            requiredContentLength_ = response.contentLength;
        }
    }

    POMDOG_ASSERT(response_.has_value());

    auto readChunks = [&]() -> std::expected<void, std::unique_ptr<Error>> {
        if (!response_->chunkedTransferEncoding) {
            return {};
        }

        while (requiredContentLength_ < rawData_.size()) {
            POMDOG_ASSERT(rawData_.size() > 0);

            if (rawData_.size() - requiredContentLength_ < 5) {
                // NOTE: The packet is still incomplete.
                // 5 bytes means the minimum size of "\r\nX\r\n" ('X' is any chunk size.)
                return {};
            }

            const auto text = asText(rawData_);
            const auto bufferBegin = std::next(text.begin(), static_cast<std::ptrdiff_t>(requiredContentLength_));
            const auto bufferEnd = text.end();
            POMDOG_ASSERT(bufferBegin != bufferEnd);

            auto iter = bufferBegin;

            if (requiredContentLength_ > 0) {
                // NOTE: Remove the end of a chunk such as "\r\n"
                iter = skip(bufferBegin, bufferEnd, '\r');
                if (!readChar(iter, bufferEnd, '\n')) {
                    return std::unexpected(errors::make("Invalid HTTP Response, TransferEncoding Chunked"));
                }
            }

            auto word = getWord(iter, bufferEnd, isLinebreak);
            if (!word) {
                return std::unexpected(errors::wrap(std::move(word.error()), "Invalid HTTP Response, TransferEncoding Chunked"));
            }

            std::stringstream ss;
            if (!(ss << std::hex << *word)) {
                return std::unexpected(errors::make("Invalid HTTP Response, TransferEncoding Chunked"));
            }
            std::size_t chunkSize = 0;
            if (!(ss >> chunkSize)) {
                return std::unexpected(errors::make("Invalid HTTP Response, TransferEncoding Chunked"));
            }

            iter = skip(iter, bufferEnd, '\r');
            if (!readChar(iter, bufferEnd, '\n')) {
                return std::unexpected(errors::make("Invalid HTTP Response, TransferEncoding Chunked"));
            }

            const auto dataOffset = std::distance(bufferBegin, iter);
            POMDOG_ASSERT(dataOffset >= 0);
            POMDOG_ASSERT(rawData_.size() >= static_cast<std::size_t>(dataOffset));

            if (chunkSize == 0) {
                // NOTE: "0\r\n" is the last chunk. The message ends with an
                // empty line, after optional trailer fields, which the parser
                // waits for and drops.
                const auto rest = std::string_view{iter, bufferEnd};
                if (!rest.starts_with("\r\n") && (rest.find("\r\n\r\n") == std::string_view::npos)) {
                    return {};
                }
                hasEOFChunk = true;
                rawData_.resize(requiredContentLength_);
                return {};
            }

            // NOTE: Remove the chunk size line, and keep the chunk data.
            const auto chunkBegin = std::next(std::begin(rawData_), static_cast<std::ptrdiff_t>(requiredContentLength_));
            rawData_.erase(chunkBegin, std::next(chunkBegin, dataOffset));
            requiredContentLength_ += chunkSize;
        }
        return {};
    };

    if (auto result = readChunks(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    if (response_->contentLength > 0) {
        if (response_->contentLength > rawData_.size()) {
            return HTTPParseResult::WouldBlock;
        }
        response_->body = std::move(rawData_);
    }
    else if (response_->chunkedTransferEncoding) {
        if (!hasEOFChunk) {
            return HTTPParseResult::WouldBlock;
        }
        // NOTE: EOF
        response_->body = std::move(rawData_);
    }

    return HTTPParseResult::EndOfFile;
}

HTTPResponse HTTPParser::takeResponse()
{
    POMDOG_ASSERT(response_.has_value());
    auto response = std::move(*response_);
    response_.reset();
    return response;
}

} // namespace pomdog::network::detail
