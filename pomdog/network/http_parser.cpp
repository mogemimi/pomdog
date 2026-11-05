// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_parser.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// Limits the status line and header fields of a response, and the trailer
/// fields of a chunked body, so that a server cannot send them without end.
constexpr std::size_t maxHeaderSize = 64 * 1024;

/// Returns bytes as text. The parser reads HTTP syntax, which is ASCII.
[[nodiscard]] std::string_view
asText(std::span<const u8> bytes) noexcept
{
    return std::string_view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};
}

[[nodiscard]] bool
isDigit(char c) noexcept
{
    return (c >= '0') && (c <= '9');
}

[[nodiscard]] char
toLower(char c) noexcept
{
    if ((c >= 'A') && (c <= 'Z')) {
        return static_cast<char>((c - 'A') + 'a');
    }
    return c;
}

[[nodiscard]] bool
equalsIgnoringCase(std::string_view a, std::string_view b) noexcept
{
    return std::ranges::equal(a, b, [](char x, char y) { return toLower(x) == toLower(y); });
}

/// Returns text without the spaces and tabs at both ends.
[[nodiscard]] std::string_view
trim(std::string_view text) noexcept
{
    const auto begin = text.find_first_not_of(" \t");
    if (begin == std::string_view::npos) {
        return {};
    }
    const auto end = text.find_last_not_of(" \t");
    return text.substr(begin, end - begin + 1);
}

/// Returns true if c may appear in a header line: a visible character, a
/// space, a tab, or a byte above 0x7F (obs-text).
[[nodiscard]] bool
isFieldCharacter(char c) noexcept
{
    const auto byte = static_cast<unsigned char>(c);
    return (byte == '\t') || ((byte >= 0x20) && (byte != 0x7F));
}

/// Parses text, which must hold only decimal digits.
[[nodiscard]] std::optional<std::size_t>
parseDecimal(std::string_view text) noexcept
{
    if (text.empty()) {
        return std::nullopt;
    }
    constexpr auto maxValue = std::numeric_limits<std::size_t>::max();
    std::size_t value = 0;
    for (const char c : text) {
        if (!isDigit(c)) {
            return std::nullopt;
        }
        const auto digit = static_cast<std::size_t>(c - '0');
        if (value > ((maxValue - digit) / 10)) {
            return std::nullopt;
        }
        value = (value * 10) + digit;
    }
    return value;
}

/// Parses the hexadecimal chunk size at the start of line, which chunk
/// extensions after ';' may follow.
[[nodiscard]] std::optional<std::size_t>
parseChunkSize(std::string_view line) noexcept
{
    constexpr auto maxValue = std::numeric_limits<std::size_t>::max();
    std::size_t value = 0;
    std::size_t count = 0;
    for (const char c : line) {
        std::size_t digit = 0;
        if (isDigit(c)) {
            digit = static_cast<std::size_t>(c - '0');
        }
        else if ((c >= 'a') && (c <= 'f')) {
            digit = static_cast<std::size_t>(c - 'a') + 10;
        }
        else if ((c >= 'A') && (c <= 'F')) {
            digit = static_cast<std::size_t>(c - 'A') + 10;
        }
        else {
            break;
        }
        // NOTE: Fail a size that does not fit, rather than let it wrap.
        if (value > (maxValue >> 4)) {
            return std::nullopt;
        }
        value = (value << 4) | digit;
        ++count;
    }
    if (count == 0) {
        return std::nullopt;
    }
    const auto rest = trim(line.substr(count));
    if (!rest.empty() && !rest.starts_with(';')) {
        return std::nullopt;
    }
    return value;
}

/// Line is a line of text without its line break, and the bytes it takes with
/// the line break.
struct Line final {
    std::string_view text;
    std::size_t size = 0;
};

/// Returns the first line of text, which ends at "\n" with an optional "\r"
/// before it, or nullopt when text has no line break.
[[nodiscard]] std::optional<Line>
takeLine(std::string_view text) noexcept
{
    const auto end = text.find('\n');
    if (end == std::string_view::npos) {
        return std::nullopt;
    }
    auto line = text.substr(0, end);
    if (line.ends_with('\r')) {
        line.remove_suffix(1);
    }
    return Line{.text = line, .size = end + 1};
}

/// Returns the value of the first field named name. Field names are
/// case-insensitive (RFC 9110, section 5.1).
[[nodiscard]] std::optional<std::string_view>
findField(const HTTPResponse& response, std::string_view name) noexcept
{
    for (const auto& [key, value] : response.headers) {
        if (equalsIgnoringCase(key, name)) {
            return value;
        }
    }
    return std::nullopt;
}

/// Parses the status line and the header fields in header, which ends with an
/// empty line.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
parseHeader(HTTPResponse& response, std::string_view header)
{
    const auto statusLine = takeLine(header);
    POMDOG_ASSERT(statusLine.has_value());
    header.remove_prefix(statusLine->size);

    // NOTE: Such as "HTTP/1.1 200 OK", where the reason phrase may be empty.
    const auto line = statusLine->text;
    if (!std::ranges::all_of(line, isFieldCharacter)) {
        return std::unexpected(errors::make("Invalid HTTP Response"));
    }
    const auto space = line.find(' ');
    const auto protocol = line.substr(0, space);
    if (!((protocol.size() >= 8) &&
            equalsIgnoringCase(protocol.substr(0, 5), "HTTP/") &&
            isDigit(protocol[5]) &&
            (protocol[6] == '.') &&
            isDigit(protocol[7]))) {
        return std::unexpected(errors::make("Invalid HTTP Response protocol"));
    }
    if (space == std::string_view::npos) {
        return std::unexpected(errors::make("Invalid HTTP Response status"));
    }
    auto status = line.substr(space + 1);
    status.remove_prefix(std::min(status.find_first_not_of(' '), status.size()));
    if ((status.size() < 3) ||
        !isDigit(status[0]) || !isDigit(status[1]) || !isDigit(status[2]) ||
        ((status.size() > 3) && (status[3] != ' '))) {
        return std::unexpected(errors::make("Invalid HTTP Response status"));
    }
    response.protocol = protocol;
    response.status = status;
    response.statusCode = ((status[0] - '0') * 100) + ((status[1] - '0') * 10) + (status[2] - '0');

    for (;;) {
        const auto field = takeLine(header);
        POMDOG_ASSERT(field.has_value());
        header.remove_prefix(field->size);
        const auto text = field->text;
        if (text.empty()) {
            break;
        }
        const auto colon = text.find(':');
        if ((colon == std::string_view::npos) || (colon == 0) || !std::ranges::all_of(text, isFieldCharacter)) {
            return std::unexpected(errors::make("Invalid HTTP Response field"));
        }
        const auto name = text.substr(0, colon);
        if (name.find_first_of(" \t") != std::string_view::npos) {
            return std::unexpected(errors::make("Invalid HTTP Response field"));
        }
        response.headers.emplace_back(std::string{name}, std::string{trim(text.substr(colon + 1))});
    }
    return {};
}

[[nodiscard]] std::unique_ptr<Error>
makeBodyTooLargeError(std::size_t maxBodySize)
{
    return errors::make("the response body is larger than " + std::to_string(maxBodySize) + " bytes");
}

} // namespace

HTTPParser::HTTPParser(HTTPMethod method, std::size_t maxBodySize) noexcept
    : maxBodySize_(maxBodySize)
    , method_(method)
{
}

std::expected<HTTPParseResult, std::unique_ptr<Error>>
HTTPParser::parse(std::span<const u8> data)
{
    if (state_ == State::Complete) {
        return HTTPParseResult::EndOfFile;
    }

    buffer_.insert(std::end(buffer_), std::begin(data), std::end(data));

    while (state_ != State::Complete) {
        auto progressed = advance();
        if (!progressed) {
            return std::unexpected(std::move(progressed.error()));
        }
        if (!*progressed) {
            break;
        }
    }

    // NOTE: Drop the read bytes once they are at least half of the buffer, so
    // that each byte moves only a few times however the data is split.
    if (offset_ == buffer_.size()) {
        buffer_.clear();
        offset_ = 0;
    }
    else if ((offset_ * 2) >= buffer_.size()) {
        buffer_.erase(std::begin(buffer_), std::next(std::begin(buffer_), static_cast<std::ptrdiff_t>(offset_)));
        offset_ = 0;
    }

    return (state_ == State::Complete) ? HTTPParseResult::EndOfFile : HTTPParseResult::WouldBlock;
}

std::expected<HTTPParseResult, std::unique_ptr<Error>>
HTTPParser::finish()
{
    if (state_ == State::Complete) {
        return HTTPParseResult::EndOfFile;
    }
    if (state_ == State::BodyUntilClose) {
        state_ = State::Complete;
        return HTTPParseResult::EndOfFile;
    }
    if (!response_.has_value() && unread().empty()) {
        return std::unexpected(errors::make("the server closed the connection before it sent a response"));
    }
    return std::unexpected(errors::make("the server closed the connection before the response ended"));
}

HTTPResponse HTTPParser::takeResponse()
{
    POMDOG_ASSERT(state_ == State::Complete);
    POMDOG_ASSERT(response_.has_value());
    auto response = std::move(*response_);
    response_.reset();
    return response;
}

std::expected<bool, std::unique_ptr<Error>>
HTTPParser::advance()
{
    switch (state_) {
    case State::Header:
        return readHeader();
    case State::FixedBody:
    case State::ChunkData:
    case State::BodyUntilClose:
        return readBody();
    case State::ChunkSize:
        return readChunkSize();
    case State::ChunkDataEnd:
        return readChunkDataEnd();
    case State::Trailer:
        return readTrailer();
    case State::Complete:
        return false;
    }
    return false;
}

std::expected<bool, std::unique_ptr<Error>>
HTTPParser::readHeader()
{
    const auto text = asText(unread());

    // NOTE: Search only the bytes that arrived since the last search, and the
    // three before them, which may start "\r\n\r\n".
    const auto searchStart = (headerScanned_ > 3) ? (headerScanned_ - 3) : 0;
    const auto end = text.find("\r\n\r\n", searchStart);
    if (end == std::string_view::npos) {
        if (text.size() > maxHeaderSize) {
            return std::unexpected(errors::make("the response header is larger than " + std::to_string(maxHeaderSize) + " bytes"));
        }
        headerScanned_ = text.size();
        return false;
    }

    const auto headerSize = end + 4;
    if (headerSize > maxHeaderSize) {
        return std::unexpected(errors::make("the response header is larger than " + std::to_string(maxHeaderSize) + " bytes"));
    }

    HTTPResponse response;
    if (auto result = parseHeader(response, text.substr(0, headerSize)); !result) {
        return std::unexpected(std::move(result.error()));
    }
    offset_ += headerSize;
    headerScanned_ = 0;

    // NOTE: An informational response, such as 103 Early Hints, comes before
    // the final response.
    if ((response.statusCode >= 100) && (response.statusCode < 200)) {
        return true;
    }

    auto& finalResponse = response_.emplace(std::move(response));

    // NOTE: These responses have no body, though their headers may describe
    // one, such as the Content-Length of a GET response to the same URL.
    const bool hasNoBody = (method_ == HTTPMethod::Head) || (finalResponse.statusCode == 204) || (finalResponse.statusCode == 304);

    // NOTE: Transfer-Encoding wins over Content-Length (RFC 9112, section
    // 6.3). A body whose last coding is not chunked lasts until the server
    // closes the connection. HTTP/1.0 has no transfer codings.
    if (equalsIgnoringCase(finalResponse.protocol, "HTTP/1.1")) {
        if (const auto field = findField(finalResponse, "Transfer-Encoding"); field.has_value()) {
            const auto comma = field->rfind(',');
            const auto lastCoding = trim((comma == std::string_view::npos) ? *field : field->substr(comma + 1));
            finalResponse.chunkedTransferEncoding = equalsIgnoringCase(lastCoding, "chunked");
            if (hasNoBody) {
                state_ = State::Complete;
            }
            else {
                state_ = finalResponse.chunkedTransferEncoding ? State::ChunkSize : State::BodyUntilClose;
            }
            return true;
        }
    }

    std::optional<std::size_t> contentLength;
    for (const auto& [name, value] : finalResponse.headers) {
        if (!equalsIgnoringCase(name, "Content-Length")) {
            continue;
        }
        const auto length = parseDecimal(value);
        if (!length.has_value() || (contentLength.has_value() && (*contentLength != *length))) {
            return std::unexpected(errors::make("Invalid HTTP Response 'Content-Length'"));
        }
        contentLength = length;
    }
    if (hasNoBody) {
        finalResponse.contentLength = contentLength.value_or(0);
        state_ = State::Complete;
        return true;
    }
    if (!contentLength.has_value()) {
        state_ = State::BodyUntilClose;
        return true;
    }
    if ((maxBodySize_ > 0) && (*contentLength > maxBodySize_)) {
        return std::unexpected(makeBodyTooLargeError(maxBodySize_));
    }
    finalResponse.contentLength = *contentLength;
    remaining_ = *contentLength;
    state_ = (remaining_ > 0) ? State::FixedBody : State::Complete;
    return true;
}

std::expected<bool, std::unique_ptr<Error>>
HTTPParser::readChunkSize()
{
    const auto text = asText(unread());
    const auto line = takeLine(text);
    if (!line.has_value()) {
        if (text.size() > maxHeaderSize) {
            return std::unexpected(errors::make("Invalid HTTP Response, the chunk size line is too long"));
        }
        return false;
    }
    const auto chunkSize = parseChunkSize(line->text);
    if (!chunkSize.has_value()) {
        return std::unexpected(errors::make("Invalid HTTP Response, TransferEncoding Chunked"));
    }
    offset_ += line->size;

    if (*chunkSize == 0) {
        // NOTE: The last chunk. Optional trailer fields and an empty line end
        // the body, and the parser drops the fields.
        trailerSize_ = 0;
        state_ = State::Trailer;
        return true;
    }

    POMDOG_ASSERT(response_.has_value());
    if ((maxBodySize_ > 0) && (*chunkSize > (maxBodySize_ - response_->body.size()))) {
        return std::unexpected(makeBodyTooLargeError(maxBodySize_));
    }
    remaining_ = *chunkSize;
    state_ = State::ChunkData;
    return true;
}

std::expected<bool, std::unique_ptr<Error>>
HTTPParser::readChunkDataEnd()
{
    const auto text = asText(unread());
    if (text.starts_with("\r\n")) {
        offset_ += 2;
    }
    else if (text.starts_with('\n')) {
        offset_ += 1;
    }
    else if (text.empty() || (text == "\r")) {
        return false;
    }
    else {
        return std::unexpected(errors::make("Invalid HTTP Response, TransferEncoding Chunked"));
    }
    state_ = State::ChunkSize;
    return true;
}

std::expected<bool, std::unique_ptr<Error>>
HTTPParser::readTrailer()
{
    const auto text = asText(unread());
    const auto line = takeLine(text);
    if (!line.has_value()) {
        if ((trailerSize_ + text.size()) > maxHeaderSize) {
            return std::unexpected(errors::make("the response trailer is larger than " + std::to_string(maxHeaderSize) + " bytes"));
        }
        return false;
    }
    trailerSize_ += line->size;
    if (trailerSize_ > maxHeaderSize) {
        return std::unexpected(errors::make("the response trailer is larger than " + std::to_string(maxHeaderSize) + " bytes"));
    }
    offset_ += line->size;
    if (line->text.empty()) {
        state_ = State::Complete;
    }
    return true;
}

std::expected<bool, std::unique_ptr<Error>>
HTTPParser::readBody()
{
    POMDOG_ASSERT(response_.has_value());
    auto& body = response_->body;

    auto data = unread();
    if (state_ != State::BodyUntilClose) {
        data = data.first(std::min(data.size(), remaining_));
    }
    if (data.empty()) {
        return false;
    }
    if ((maxBodySize_ > 0) && (data.size() > (maxBodySize_ - body.size()))) {
        return std::unexpected(makeBodyTooLargeError(maxBodySize_));
    }

    body.insert(std::end(body), std::begin(data), std::end(data));
    offset_ += data.size();

    if (state_ != State::BodyUntilClose) {
        remaining_ -= data.size();
        if (remaining_ == 0) {
            state_ = (state_ == State::FixedBody) ? State::Complete : State::ChunkDataEnd;
        }
    }
    return true;
}

std::span<const u8>
HTTPParser::unread() const noexcept
{
    return std::span<const u8>{buffer_}.subspan(offset_);
}

} // namespace pomdog::network::detail
