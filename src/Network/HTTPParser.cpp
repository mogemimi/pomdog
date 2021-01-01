// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "HTTPParser.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/HTTPResponse.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <optional>
#include <sstream>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
namespace {

template <class StringIterator>
std::tuple<std::string_view, StringIterator, std::unique_ptr<Error>>
GetWord(StringIterator begin, StringIterator end, std::function<bool(char)> isSeparator)
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
            return std::make_tuple(word, it, Errors::New("Invalid HTTP Response"));
            break;
        }
        ++count;
        word = std::string_view{&(*begin), count};
    }
    return std::make_tuple(word, it, nullptr);
}

template <class StringIterator>
StringIterator
Skip(StringIterator begin, StringIterator end, char skip)
{
    auto it = begin;

    // NOTE: Skip white-spaces
    for (; it != end; ++it) {
        if (*it != skip) {
            break;
        }
    }
    return it;
}

char ToLower(char c) noexcept
{
    static_assert('A' < 'Z');
    static_assert('A' < 'a');
    if ((c >= 'A') && (c <= 'Z')) {
        return (c - 'A') + 'a';
    }
    return c;
}

bool IsDigit(char c) noexcept
{
    static_assert('0' < '9');
    return (c >= '0') && (c <= '9');
}

bool CompareCaseInsensitive(std::string_view a, std::string_view b) noexcept
{
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b),
        [](char x, char y) {
            return (ToLower(x) == ToLower(y));
        });
}

template <class StringIterator>
bool ReadChar(StringIterator& it, StringIterator end, char character)
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
ParseFields(std::vector<char>::const_iterator& iter, std::vector<char>::const_iterator end)
{
    std::vector<std::pair<std::string, std::string>> fields;

    std::string_view line;
    std::unique_ptr<Error> err;

    while (iter != end) {
        std::tie(line, iter, err) = GetWord<std::vector<char>::const_iterator>(iter, end, isLinebreak);
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
                return std::make_tuple(std::move(fields), Errors::New("Invalid HTTP Response field"));
            }
        }

        auto key = line.substr(0, pos);
        auto value = line.substr(pos + 2);
        fields.emplace_back(key, value);

        iter = Skip(iter, end, '\r');
        if (!ReadChar(iter, end, '\n')) {
            return std::make_tuple(std::move(fields), Errors::New("Invalid HTTP Response field"));
        }
    }

    return std::make_tuple(std::move(fields), nullptr);
}

template <typename T>
std::optional<std::string_view> FindField(T& fields, std::string_view key)
{
    auto iter = std::find_if(std::begin(fields), std::end(fields),
        [&](const std::pair<std::string, std::string>& field) -> bool {
            return field.first == key;
        });

    if (iter != std::end(fields)) {
        return iter->second;
    }
    return std::nullopt;
}

std::unique_ptr<Error>
ParseResponse(HTTPResponse& response, std::vector<char>& rawData)
{
    auto [word, iter, err] = GetWord<std::vector<char>::const_iterator>(std::begin(rawData), std::end(rawData), isSpace);
    if (err != nullptr) {
        return std::move(err);
    }

    if ((word.size() >= 8) &&
        (word[6] == '.') &&
        (CompareCaseInsensitive(word.substr(0, 5), "HTTP/")) &&
        IsDigit(word[5]) &&
        IsDigit(word[7])) {
        response.Protocol = word;
    }
    else {
        return Errors::New("Invalid HTTP Response protocol");
    }

    std::string_view status;

    iter = Skip<std::vector<char>::const_iterator>(iter, std::end(rawData), ' ');
    std::tie(status, iter, err) = GetWord<std::vector<char>::const_iterator>(iter, std::end(rawData), isLinebreak);
    if (err != nullptr) {
        return std::move(err);
    }

    if ((status.size() >= 4) && (status[3] == ' ')) {
        std::stringstream ss;
        ss << status[0] << status[1] << status[2];
        if (!(ss >> response.StatusCode)) {
            return Errors::New("Invalid HTTP Response status");
        }
        response.Status = status;
    }
    else {
        return Errors::New("Invalid HTTP Response status");
    }

    iter = Skip<std::vector<char>::const_iterator>(iter, std::end(rawData), '\r');
    if (!ReadChar<std::vector<char>::const_iterator>(iter, std::end(rawData), '\n')) {
        return Errors::New("Invalid HTTP Response");
    }

    std::tie(response.Header, err) = ParseFields(iter, std::end(rawData));
    if (err != nullptr) {
        return std::move(err);
    }

    iter = Skip<std::vector<char>::const_iterator>(iter, std::end(rawData), '\r');
    if (!ReadChar<std::vector<char>::const_iterator>(iter, std::end(rawData), '\n')) {
        return Errors::New("Invalid HTTP Response");
    }

    if (auto field = FindField(response.Header, "Content-Length"); field.has_value()) {
        std::stringstream ss;
        ss << *field;
        size_t length;
        if (!(ss >> length)) {
            return Errors::New("Invalid HTTP Response 'Content-Length'");
        }
        response.ContentLength = length;
    }

    if (CompareCaseInsensitive(response.Protocol, "HTTP/1.1")) {
        if (auto field = FindField(response.Header, "Transfer-Encoding"); field.has_value()) {
            response.ChunkedTransferEncoding = (field->find("chunked") != std::string::npos);
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

bool
IsResponseHeaderReadable(const std::vector<char>& rawData)
{
    std::string_view s{rawData.data(), rawData.size()};
    if (auto pos = s.find("\r\n\r\n"); pos != std::string_view::npos) {
        return true;
    }
    return false;
}

} // namespace

std::tuple<HTTPParseResult, std::unique_ptr<Error>>
HTTPParser::Parse(const ArrayView<std::uint8_t>& view)
{
    bool hasEOFChunk = false;

    if (response == nullptr) {
        const auto bufSize = rawData.size();
        rawData.resize(bufSize + view.GetSize());
        std::memcpy(rawData.data() + bufSize, view.GetData(), view.GetSize());

        if (IsResponseHeaderReadable(rawData)) {
            response = std::make_unique<HTTPResponse>();

            if (auto err = ParseResponse(*response, rawData); err != nullptr) {
                return std::make_tuple(HTTPParseResult::Error, std::move(err));
            }

            if (response->ChunkedTransferEncoding) {
                requiredContentLength = 0;
            }
            else {
                requiredContentLength = response->ContentLength;
            }
        }
    }
    else {
        POMDOG_ASSERT(response != nullptr);

        if (!view.IsEmpty()) {
            const auto bufSize = rawData.size();
            rawData.resize(bufSize + view.GetSize());
            std::memcpy(rawData.data() + bufSize, view.GetData(), view.GetSize());
        }
    }

    if (response == nullptr) {
        return std::make_tuple(HTTPParseResult::WouldBlock, nullptr);
    }

    auto readResponse = [&]() -> std::unique_ptr<Error> {
        if (!response->ChunkedTransferEncoding) {
            return nullptr;
        }

        while (requiredContentLength < rawData.size()) {
            POMDOG_ASSERT(rawData.size() > 0);

            if (rawData.size() - requiredContentLength < 5) {
                // NOTE: The packet is still incomplete.
                // 5 bytes means the minimum size of "\r\nX\r\n" ('X' is any chunk size.)
                return nullptr;
            }

            const auto bufferBegin = std::next(std::cbegin(rawData), requiredContentLength);
            const auto bufferEnd = std::cend(rawData);
            POMDOG_ASSERT(bufferBegin != bufferEnd);

            auto iter = bufferBegin;

            if (requiredContentLength > 0) {
                // NOTE: Remove the end of a chunk such as "\r\n"
                iter = Skip<std::vector<char>::const_iterator>(bufferBegin, bufferEnd, '\r');
                if (!ReadChar<std::vector<char>::const_iterator>(iter, bufferEnd, '\n')) {
                    return Errors::New("Invalid HTTP Response, TransferEncoding Chunked");
                }
            }

            std::string_view word;
            std::unique_ptr<Error> err;

            std::tie(word, iter, err) = GetWord<std::vector<char>::const_iterator>(iter, bufferEnd, isLinebreak);

            if (err != nullptr) {
                return Errors::Wrap(std::move(err), "Invalid HTTP Response, TransferEncoding Chunked");
            }

            std::stringstream ss;
            if (!(ss << std::hex << word)) {
                return Errors::New("Invalid HTTP Response, TransferEncoding Chunked");
            }
            std::size_t chunkSize = 0;
            if (!(ss >> chunkSize)) {
                return Errors::New("Invalid HTTP Response, TransferEncoding Chunked");
            }

            iter = Skip<std::vector<char>::const_iterator>(iter, bufferEnd, '\r');
            if (!ReadChar<std::vector<char>::const_iterator>(iter, bufferEnd, '\n')) {
                return Errors::New("Invalid HTTP Response, TransferEncoding Chunked");
            }

            const auto dataOffset = static_cast<std::size_t>(std::distance(bufferBegin, iter));

            if (chunkSize == 0) {
                // NOTE: "0\r\n\r\n" is an EOF chunk
                hasEOFChunk = true;

                POMDOG_ASSERT(rawData.size() >= dataOffset);
                rawData.resize(rawData.size() - dataOffset);
                return nullptr;
            }

            requiredContentLength += chunkSize;

            POMDOG_ASSERT(rawData.size() >= dataOffset);
            rawData.erase(bufferBegin, iter);
        }
        return nullptr;
    };

    if (auto err = readResponse(); err != nullptr) {
        return std::make_tuple(HTTPParseResult::Error, std::move(err));
    }

    if (response->ContentLength > 0) {
        if (response->ContentLength > rawData.size()) {
            return std::make_tuple(HTTPParseResult::WouldBlock, nullptr);
        }
        response->Body = std::move(rawData);
    }
    else if (response->ChunkedTransferEncoding) {
        if (!hasEOFChunk) {
            return std::make_tuple(HTTPParseResult::WouldBlock, nullptr);
        }
        // NOTE: EOF
        response->Body = std::move(rawData);
    }

    return std::make_tuple(HTTPParseResult::EndOfFile, nullptr);
}

std::unique_ptr<HTTPResponse> HTTPParser::GetResponse()
{
    return std::move(response);
}

} // namespace Pomdog::Detail
