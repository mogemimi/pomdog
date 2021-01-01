// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/StringHelper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <regex>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::PathHelper {
namespace {

[[nodiscard]] std::string_view::size_type
findFirstOfSlash(
    std::string_view path,
    std::string_view::size_type first = 0) noexcept
{
    POMDOG_ASSERT(first != std::string::npos);
    auto index = path.find_first_of('/', first);
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    const auto windowsIndex = path.find_first_of('\\', first);
    if (index != std::string::npos) {
        if (windowsIndex != std::string::npos) {
            index = std::min(index, windowsIndex);
        }
    }
    else if (windowsIndex != std::string::npos) {
        POMDOG_ASSERT(index == std::string::npos);
        index = windowsIndex;
    }
#endif
    if (index == 0) {
        return index + 1;
    }
    return index;
}

[[nodiscard]] bool
isRootDirectoryName(std::string_view name) noexcept
{
    if (name == "/") {
        return true;
    }
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    // NOTE: Windows's drive (ex. 'C:' drive)
    std::regex windowsDriveRegex(R"(^[A-Za-z]:)");
    if (std::regex_match(name.cbegin(), name.cend(), windowsDriveRegex)) {
        return true;
    }
    // TODO: Add windows's '\\[server name]' support.
#endif
    return false;
}

class PathIterator final {
    std::string_view source;
    std::string_view::size_type startPos = std::string::npos;
    std::string_view::size_type endPos = std::string::npos;

public:
    [[maybe_unused]] PathIterator() noexcept = default;

    PathIterator(std::string_view sourceIn, std::string_view::size_type startIn, std::string_view::size_type endIn) noexcept
        : source(sourceIn)
        , startPos(startIn)
        , endPos(endIn)
    {
    }

    std::string_view operator*() const noexcept
    {
        if (source.empty()) {
            return source;
        }
        POMDOG_ASSERT(startPos != std::string_view::npos);
        POMDOG_ASSERT(endPos != std::string_view::npos);
        if (endPos == std::string_view::npos) {
            return source.substr(startPos);
        }
        return source.substr(startPos, endPos - startPos);
    }

    [[maybe_unused]] bool operator==(const PathIterator& iter) const noexcept
    {
        return (source.data() == iter.source.data()) &&
               (startPos == iter.startPos) &&
               (endPos == iter.endPos);
    }

    bool operator!=(const PathIterator& iter) const noexcept
    {
        return (source.data() != iter.source.data()) ||
               (startPos != iter.startPos) ||
               (endPos != iter.endPos);
    }

    static PathIterator begin(const std::string_view& path) noexcept
    {
        if (path.empty()) {
            return PathIterator(path, std::string_view::npos, std::string_view::npos);
        }
        const auto index = findFirstOfSlash(path);
        if (index == std::string_view::npos) {
            return PathIterator(path, 0, path.size());
        }
        return PathIterator(path, 0, index);
    }

    static PathIterator end(const std::string_view& path) noexcept
    {
        return PathIterator(path, std::string_view::npos, std::string_view::npos);
    }

    static PathIterator next(const PathIterator& iter) noexcept
    {
        POMDOG_ASSERT(iter.startPos != std::string_view::npos);
        POMDOG_ASSERT(iter.endPos != std::string_view::npos);
        auto endPos = iter.endPos;
        if (endPos == iter.source.size()) {
            return PathIterator(iter.source, std::string_view::npos, std::string_view::npos);
        }

        if (iter.source.at(endPos) == '/' || iter.source.at(endPos) == '\\') {
            if (endPos < iter.source.size()) {
                endPos++;
            }
        }
        const auto index = findFirstOfSlash(iter.source, endPos);
        if (index == std::string_view::npos) {
            if (endPos < iter.source.size()) {
                return PathIterator(iter.source, endPos, iter.source.size());
            }
            return PathIterator(iter.source, index, index);
        }
        return PathIterator(iter.source, endPos, index);
    }
};

} // namespace

[[nodiscard]] std::string
Join(std::string_view path1, std::string_view path2) noexcept
{
    std::string result = std::string(path1);
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (!result.empty() && '\\' == result.back()) {
        result.erase(std::prev(std::end(result)));
        result += '\\';
    }
#endif
    if (!result.empty() && '/' != result.back()) {
        if (!path2.empty() && '/' != path2.front()) {
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
            result += '\\';
#else
            result += '/';
#endif
        }
    }
    result += path2;
    return result;
}

[[nodiscard]] std::string_view
GetBaseName(std::string_view path) noexcept
{
    const auto lastIndex = path.find_last_of('/');
    if (std::string_view::npos != lastIndex) {
        return path.substr(lastIndex + 1);
    }
    return path;
}

[[nodiscard]] std::string_view
GetDirectoryName(std::string_view path) noexcept
{
    if (!path.empty() && path.back() == '/') {
        return path;
    }
    const auto lastIndex = path.find_last_of('/');
    if (std::string::npos != lastIndex) {
        return path.substr(0, lastIndex);
    }
    return {};
}

[[nodiscard]] std::tuple<std::string_view, std::string_view>
Split(std::string_view path) noexcept
{
    std::tuple<std::string_view, std::string_view> result;
    auto lastIndex = path.find_last_of('/');
    if (std::string_view::npos != lastIndex) {
        std::get<0>(result) = path.substr(0, lastIndex);
        std::get<1>(result) = path.substr(lastIndex + 1);
    }
    else {
        std::get<1>(result) = path;
    }
    return result;
}

[[nodiscard]] std::tuple<std::string_view, std::string_view>
SplitExtension(std::string_view path) noexcept
{
    std::tuple<std::string_view, std::string_view> result;
    auto lastIndex = path.find_last_of('.');
    if (std::string_view::npos != lastIndex) {
        std::get<0>(result) = path.substr(0, lastIndex);
        std::get<1>(result) = path.substr(lastIndex + 1);
    }
    else {
        std::get<0>(result) = path;
    }
    return result;
}

[[nodiscard]] std::string
Normalize(std::string_view path) noexcept
{
    std::vector<std::string_view> paths;
    auto iter = PathIterator::begin(path);
    while (iter != PathIterator::end(path)) {
        auto current = *iter;
        iter = PathIterator::next(iter);

        if (!paths.empty() && (current == ".")) {
            continue;
        }
        if (current == "..") {
            if ((paths.size() >= 2) && (paths.back() != "..")) {
                paths.pop_back();
                continue;
            }
            if (!paths.empty() && (paths.back() == ".")) {
                paths.pop_back();
            }
            if ((paths.size() == 1) && (paths.back() == "/")) {
                continue;
            }
        }
        paths.push_back(current);
    }

    std::string fullPath;
    for (const auto& current : paths) {
        if (fullPath.empty() && isRootDirectoryName(current)) {
            fullPath = current;
            continue;
        }
        fullPath = PathHelper::Join(fullPath, current);
    }
    return fullPath;
}

[[nodiscard]] std::string
ToSlash(std::string_view path) noexcept
{
    return StringHelper::ReplaceAll(path, "\\", "/");
}

[[nodiscard]] std::string
Relative(std::string_view path, std::string_view start) noexcept
{
    const auto fullPath = PathHelper::Normalize(path);
    const auto fullPathStart = PathHelper::Normalize(start);

    auto iterL = PathIterator::begin(fullPath);
    auto iterR = PathIterator::begin(fullPathStart);
    while ((iterL != PathIterator::end(fullPath)) &&
           (iterR != PathIterator::end(fullPathStart))) {
        if (*iterL != *iterR) {
            break;
        }
        iterR = PathIterator::next(iterR);
        iterL = PathIterator::next(iterL);
    }

    std::string result;
    while (iterR != PathIterator::end(fullPathStart)) {
        result = PathHelper::Join(result, "..");
        iterR = PathIterator::next(iterR);
    }
    while (iterL != PathIterator::end(fullPath)) {
        result = PathHelper::Join(result, *iterL);
        iterL = PathIterator::next(iterL);
    }
    if (result.empty()) {
        result = '.';
    }
    return result;
}

[[nodiscard]] bool
IsAbsolute(std::string_view path) noexcept
{
    // NOTE: See https://msdn.microsoft.com/en-us/library/bb773660.aspx
    if (path.empty()) {
        return false;
    }
    if (path.front() == '/') {
        return true;
    }
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (StringHelper::HasPrefix(path, "\\\\")) {
        // NOTE: UNC paths
        return true;
    }
    auto pathStr = std::string{path};
    if (auto iter = PathIterator::begin(pathStr); iter != PathIterator::end(pathStr)) {
        return isRootDirectoryName(*iter);
    }
#endif
    return false;
}

} // namespace Pomdog::PathHelper
