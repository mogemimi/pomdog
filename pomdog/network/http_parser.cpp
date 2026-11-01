// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_parser.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/http_response.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <optional>
#include <sstream>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

template <class StringIterator>
std::tuple<std::string_view, StringIterator, std::unique_ptr<Error>>
getWord(StringIterator begin, StringIterator end, std::function<bool(char)> isSeparator)
{
    std::string_view word;
    size_t count = 0;

    auto it = begin;

    for (; it != end; ++it) {
        const auto c = *it;
        if (isSeparator(c)) {
            break;
        }
        if (c <= 31) {
            return std::make_tuple(word, it, errors::make("Invalid HTTP Response"));
            break;
        }
        ++count;
        word = std::string_view{&(*begin), count};
    }
    return std::make_tuple(word, it, nullptr);
}

template <class StringIterator>
StringIterator
skip(StringIterator begin, StringIterator end, char character)
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

template <class StringIterator>
bool readChar(StringIterator& it, StringIterator end, char character)
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

std::tuple<std::vector<std::pair<std::string, std::string>>, std::unique_ptr<Error>>
parseFields(std::vector<char>::const_iterator& iter, std::vector<char>::const_iterator end)
{
    std::vector<std::pair<std::string, std::string>> fields;

    std::string_view line;
    std::unique_ptr<Error> err;

    while (iter != end) {
        std::tie(line, iter, err) = getWord<std::vector<char>::const_iterator>(iter, end, isLinebreak);
        if (err != nullptr) {
            return std::make_tuple(std::move(fields), std::move(err));
        }

        auto pos = line.find(": ");
        if (pos == std::string_view::npos) {
            if (line.empty()) {
                // NOTE: end of field
                break;
            }
            else {
                return std::make_tuple(std::move(fields), errors::make("Invalid HTTP Response field"));
            }
        }

        auto key = line.substr(0, pos);
        auto value = line.substr(pos + 2);
        fields.emplace_back(key, value);

        iter = skip(iter, end, '\r');
        if (!readChar(iter, end, '\n')) {
            return std::make_tuple(std::move(fields), errors::make("Invalid HTTP Response field"));
        }
    }

    return std::make_tuple(std::move(fields), nullptr);
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

std::unique_ptr<Error>
parseResponse(HTTPResponse& response, std::vector<char>& rawData)
{
    auto [word, iter, err] = getWord<std::vector<char>::const_iterator>(std::begin(rawData), std::end(rawData), isSpace);
    if (err != nullptr) {
        return std::move(err);
    }

    if ((word.size() >= 8) &&
        (word[6] == '.') &&
        (compareCaseInsensitive(word.substr(0, 5), "HTTP/")) &&
        isDigit(word[5]) &&
        isDigit(word[7])) {
        response.protocol = word;
    }
    else {
        return errors::make("Invalid HTTP Response protocol");
    }

    std::string_view status;

    iter = skip<std::vector<char>::const_iterator>(iter, std::end(rawData), ' ');
    std::tie(status, iter, err) = getWord<std::vector<char>::const_iterator>(iter, std::end(rawData), isLinebreak);
    if (err != nullptr) {
        return std::move(err);
    }

    if ((status.size() >= 4) && (status[3] == ' ')) {
        std::stringstream ss;
        ss << status[0] << status[1] << status[2];
        if (!(ss >> response.statusCode)) {
            return errors::make("Invalid HTTP Response status");
        }
        response.status = status;
    }
    else {
        return errors::make("Invalid HTTP Response status");
    }

    iter = skip<std::vector<char>::const_iterator>(iter, std::end(rawData), '\r');
    if (!readChar<std::vector<char>::const_iterator>(iter, std::end(rawData), '\n')) {
        return errors::make("Invalid HTTP Response");
    }

    std::tie(response.header, err) = parseFields(iter, std::end(rawData));
    if (err != nullptr) {
        return std::move(err);
    }

    iter = skip<std::vector<char>::const_iterator>(iter, std::end(rawData), '\r');
    if (!readChar<std::vector<char>::const_iterator>(iter, std::end(rawData), '\n')) {
        return errors::make("Invalid HTTP Response");
    }

    if (auto field = findField(response.header, "Content-Length"); field.has_value()) {
        std::stringstream ss;
        ss << *field;
        size_t length;
        if (!(ss >> length)) {
            return errors::make("Invalid HTTP Response 'Content-Length'");
        }
        response.contentLength = length;
    }

    if (compareCaseInsensitive(response.protocol, "HTTP/1.1")) {
        if (auto field = findField(response.header, "Transfer-Encoding"); field.has_value()) {
            response.chunkedTransferEncoding = (field->find("chunked") != std::string::npos);
        }
    }

    if (iter != std::end(rawData)) {
        rawData.erase(std::begin(rawData), iter);
    }
    else {
        rawData.clear();
    }

    return nullptr;
}

bool isResponseHeaderReadable(const std::vector<char>& rawData)
{
    std::string_view s{rawData.data(), rawData.size()};
    if (auto pos = s.find("\r\n\r\n"); pos != std::string_view::npos) {
        return true;
    }
    return false;
}

} // namespace

std::tuple<HTTPParseResult, std::unique_ptr<Error>>
HTTPParser::parse(std::span<u8> view)
{
    bool hasEOFChunk = false;

    if (response_ == nullptr) {
        const auto bufSize = rawData_.size();
        rawData_.resize(bufSize + view.size());
        std::memcpy(rawData_.data() + bufSize, view.data(), view.size());

        if (isResponseHeaderReadable(rawData_)) {
            response_ = std::make_unique<HTTPResponse>();

            if (auto err = parseResponse(*response_, rawData_); err != nullptr) {
                return std::make_tuple(HTTPParseResult::Error, std::move(err));
            }

            if (response_->chunkedTransferEncoding) {
                requiredContentLength_ = 0;
            }
            else {
                requiredContentLength_ = response_->contentLength;
            }
        }
    }
    else {
        POMDOG_ASSERT(response_ != nullptr);

        if (!view.empty()) {
            const auto bufSize = rawData_.size();
            rawData_.resize(bufSize + view.size());
            std::memcpy(rawData_.data() + bufSize, view.data(), view.size());
        }
    }

    if (response_ == nullptr) {
        return std::make_tuple(HTTPParseResult::WouldBlock, nullptr);
    }

    auto readResponse = [&]() -> std::unique_ptr<Error> {
        if (!response_->chunkedTransferEncoding) {
            return nullptr;
        }

        while (requiredContentLength_ < rawData_.size()) {
            POMDOG_ASSERT(rawData_.size() > 0);

            if (rawData_.size() - requiredContentLength_ < 5) {
                // NOTE: The packet is still incomplete.
                // 5 bytes means the minimum size of "\r\nX\r\n" ('X' is any chunk size.)
                return nullptr;
            }

            const auto bufferBegin = std::next(std::cbegin(rawData_), requiredContentLength_);
            const auto bufferEnd = std::cend(rawData_);
            POMDOG_ASSERT(bufferBegin != bufferEnd);

            auto iter = bufferBegin;

            if (requiredContentLength_ > 0) {
                // NOTE: Remove the end of a chunk such as "\r\n"
                iter = skip<std::vector<char>::const_iterator>(bufferBegin, bufferEnd, '\r');
                if (!readChar<std::vector<char>::const_iterator>(iter, bufferEnd, '\n')) {
                    return errors::make("Invalid HTTP Response, TransferEncoding Chunked");
                }
            }

            std::string_view word;
            std::unique_ptr<Error> err;

            std::tie(word, iter, err) = getWord<std::vector<char>::const_iterator>(iter, bufferEnd, isLinebreak);

            if (err != nullptr) {
                return errors::wrap(std::move(err), "Invalid HTTP Response, TransferEncoding Chunked");
            }

            std::stringstream ss;
            if (!(ss << std::hex << word)) {
                return errors::make("Invalid HTTP Response, TransferEncoding Chunked");
            }
            std::size_t chunkSize = 0;
            if (!(ss >> chunkSize)) {
                return errors::make("Invalid HTTP Response, TransferEncoding Chunked");
            }

            iter = skip<std::vector<char>::const_iterator>(iter, bufferEnd, '\r');
            if (!readChar<std::vector<char>::const_iterator>(iter, bufferEnd, '\n')) {
                return errors::make("Invalid HTTP Response, TransferEncoding Chunked");
            }

            const auto dataOffset = static_cast<std::size_t>(std::distance(bufferBegin, iter));

            if (chunkSize == 0) {
                // NOTE: "0\r\n\r\n" is an EOF chunk
                hasEOFChunk = true;

                POMDOG_ASSERT(rawData_.size() >= dataOffset);
                rawData_.resize(rawData_.size() - dataOffset);
                return nullptr;
            }

            requiredContentLength_ += chunkSize;

            POMDOG_ASSERT(rawData_.size() >= dataOffset);
            rawData_.erase(bufferBegin, iter);
        }
        return nullptr;
    };

    if (auto err = readResponse(); err != nullptr) {
        return std::make_tuple(HTTPParseResult::Error, std::move(err));
    }

    if (response_->contentLength > 0) {
        if (response_->contentLength > rawData_.size()) {
            return std::make_tuple(HTTPParseResult::WouldBlock, nullptr);
        }
        response_->body = std::move(rawData_);
    }
    else if (response_->chunkedTransferEncoding) {
        if (!hasEOFChunk) {
            return std::make_tuple(HTTPParseResult::WouldBlock, nullptr);
        }
        // NOTE: EOF
        response_->body = std::move(rawData_);
    }

    return std::make_tuple(HTTPParseResult::EndOfFile, nullptr);
}

std::unique_ptr<HTTPResponse> HTTPParser::getResponse()
{
    return std::move(response_);
}

} // namespace pomdog::detail
