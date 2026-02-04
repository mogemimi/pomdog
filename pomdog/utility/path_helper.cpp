// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/path_helper.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::filepaths {
namespace {

[[nodiscard]] std::string_view::size_type
findFirstOfSlashWindows(
    std::string_view path,
    std::string_view::size_type first = 0) noexcept
{
    POMDOG_ASSERT(first != std::string::npos);
    auto index = path.find_first_of('/', first);
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
    if (index == 0) {
        return index + 1;
    }
    return index;
}

[[nodiscard]] std::string_view::size_type
findFirstOfSlashUnix(
    std::string_view path,
    std::string_view::size_type first = 0) noexcept
{
    POMDOG_ASSERT(first != std::string::npos);
    auto index = path.find_first_of('/', first);
    if (index == 0) {
        return index + 1;
    }
    return index;
}

[[nodiscard]] bool
isRootDirectoryNameWindows(std::string_view name) noexcept
{
    if (name == "/") {
        return true;
    }
    // NOTE: Windows's drive (ex. 'C:' drive)
    if (name.size() >= 2 &&
        ((name[0] >= 'A' && name[0] <= 'Z') || (name[0] >= 'a' && name[0] <= 'z')) &&
        name[1] == ':') {
        return true;
    }
    // TODO: Add windows's '\\[server name]' support.
    return false;
}

[[nodiscard]] bool
isRootDirectoryNameUnix(std::string_view name) noexcept
{
    if (name == "/") {
        return true;
    }
    return false;
}

struct WindowsSlashFinder {
    std::string_view::size_type operator()(std::string_view path, std::string_view::size_type first = 0) const noexcept
    {
        return findFirstOfSlashWindows(path, first);
    }
};

struct UnixSlashFinder {
    std::string_view::size_type operator()(std::string_view path, std::string_view::size_type first = 0) const noexcept
    {
        return findFirstOfSlashUnix(path, first);
    }
};

template <typename SlashFinder>
class PathIterator final {
    std::string_view source;
    std::string_view::size_type startPos = std::string::npos;
    std::string_view::size_type endPos = std::string::npos;
    SlashFinder slashFinder;

public:
    [[maybe_unused]] PathIterator() noexcept = default;

    PathIterator(std::string_view sourceIn, std::string_view::size_type startIn, std::string_view::size_type endIn, SlashFinder finder = SlashFinder{}) noexcept
        : source(sourceIn)
        , startPos(startIn)
        , endPos(endIn)
        , slashFinder(finder)
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

    static PathIterator begin(const std::string_view& path, SlashFinder finder = SlashFinder{}) noexcept
    {
        if (path.empty()) {
            return PathIterator(path, std::string_view::npos, std::string_view::npos, finder);
        }
        const auto index = finder(path, 0);
        if (index == std::string_view::npos) {
            return PathIterator(path, 0, path.size(), finder);
        }
        return PathIterator(path, 0, index, finder);
    }

    static PathIterator end(const std::string_view& path, SlashFinder finder = SlashFinder{}) noexcept
    {
        return PathIterator(path, std::string_view::npos, std::string_view::npos, finder);
    }

    static PathIterator next(const PathIterator& iter) noexcept
    {
        POMDOG_ASSERT(iter.startPos != std::string_view::npos);
        POMDOG_ASSERT(iter.endPos != std::string_view::npos);
        auto endPos = iter.endPos;
        if (endPos == iter.source.size()) {
            return PathIterator(iter.source, std::string_view::npos, std::string_view::npos, iter.slashFinder);
        }

        if (iter.source.at(endPos) == '/' || iter.source.at(endPos) == '\\') {
            if (endPos < iter.source.size()) {
                endPos++;
            }
        }
        const auto index = iter.slashFinder(iter.source, endPos);
        if (index == std::string_view::npos) {
            if (endPos < iter.source.size()) {
                return PathIterator(iter.source, endPos, iter.source.size(), iter.slashFinder);
            }
            return PathIterator(iter.source, index, index, iter.slashFinder);
        }
        return PathIterator(iter.source, endPos, index, iter.slashFinder);
    }
};

using PathIteratorWindows = PathIterator<WindowsSlashFinder>;
using PathIteratorUnix = PathIterator<UnixSlashFinder>;

[[nodiscard]] std::string
joinWindowsInternal(std::string_view path1, std::string_view path2) noexcept
{
    if (path1.empty()) {
        return std::string(path2);
    }
    if (path2.empty()) {
        return std::string(path1);
    }
    std::string result = std::string(path1);
    if (result.back() != '/' && result.back() != '\\') {
        result += '\\';
    }
    result += path2;
    return result;
}

[[nodiscard]] std::string
joinUnixInternal(std::string_view path1, std::string_view path2) noexcept
{
    if (path1.empty()) {
        return std::string(path2);
    }
    if (path2.empty()) {
        return std::string(path1);
    }
    std::string result = std::string(path1);
    if (result.back() != '/') {
        result += '/';
    }
    result += path2;
    return result;
}

template <typename PathIter, typename JoinInternalFunc, typename IsRootFunc>
[[nodiscard]] std::string
normalizeImpl(
    std::string_view path,
    JoinInternalFunc joinFunc,
    IsRootFunc isRootFunc,
    char rootSeparator) noexcept
{
    std::vector<std::string_view> paths;
    auto iter = PathIter::begin(path);
    while (iter != PathIter::end(path)) {
        auto current = *iter;
        iter = PathIter::next(iter);

        if (current == ".") {
            continue;
        }
        if (current == "..") {
            if (!paths.empty() && (paths.back() != "..") && (paths.back() != "/")) {
                paths.pop_back();
                continue;
            }
            if (!paths.empty() && (paths.back() == ".")) {
                paths.pop_back();
            }
            // NOTE: Don't add ".." if we're at root
            if (!paths.empty() && (paths.back() == "/")) {
                continue;
            }
        }
        paths.push_back(current);
    }

    std::string fullPath;
    for (const auto& current : paths) {
        if (fullPath.empty() && isRootFunc(current)) {
            // NOTE: Convert leading "/" to root separator for the platform
            if (current == "/") {
                fullPath = rootSeparator;
            }
            else {
                fullPath = current;
            }
            continue;
        }
        fullPath = joinFunc(fullPath, current);
    }
    if (fullPath.empty()) {
        fullPath = ".";
    }
    return fullPath;
}

template <typename PathIter, typename JoinInternalFunc>
[[nodiscard]] std::string
relativeImpl(
    std::string_view path,
    std::string_view start,
    JoinInternalFunc joinFunc) noexcept
{
    auto iterL = PathIter::begin(path);
    auto iterR = PathIter::begin(start);
    while ((iterL != PathIter::end(path)) &&
           (iterR != PathIter::end(start))) {
        if (*iterL != *iterR) {
            break;
        }
        iterR = PathIter::next(iterR);
        iterL = PathIter::next(iterL);
    }

    std::string result;
    while (iterR != PathIter::end(start)) {
        result = joinFunc(result, "..");
        iterR = PathIter::next(iterR);
    }
    while (iterL != PathIter::end(path)) {
        result = joinFunc(result, *iterL);
        iterL = PathIter::next(iterL);
    }
    if (result.empty()) {
        result = '.';
    }
    return result;
}

} // namespace

[[nodiscard]] std::string
joinWindows(std::string_view path1, std::string_view path2) noexcept
{
    if (path1.empty() && path2.empty()) {
        return "";
    }
    return normalizeWindows(joinWindowsInternal(path1, path2));
}

[[nodiscard]] std::string
joinUnix(std::string_view path1, std::string_view path2) noexcept
{
    if (path1.empty() && path2.empty()) {
        return "";
    }
    return normalizeUnix(joinUnixInternal(path1, path2));
}

[[nodiscard]] std::string
join(std::string_view path1, std::string_view path2) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    return joinWindows(path1, path2);
#else
    return joinUnix(path1, path2);
#endif
}

[[nodiscard]] std::string_view
getBaseName(std::string_view path) noexcept
{
    const auto lastIndex = path.find_last_of('/');
    if (std::string_view::npos != lastIndex) {
        return path.substr(lastIndex + 1);
    }
    return path;
}

[[nodiscard]] std::string_view
getDirectoryName(std::string_view path) noexcept
{
    if (path.empty()) {
        return path;
    }
    const auto lastIndex = path.find_last_of('/');
    if (lastIndex == 0) {
        // NOTE: return root directory "/"
        return path.substr(0, 1);
    }
    if (std::string::npos != lastIndex) {
        return path.substr(0, lastIndex);
    }
    return {};
}

[[nodiscard]] std::tuple<std::string_view, std::string_view>
split(std::string_view path) noexcept
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
splitExtension(std::string_view path) noexcept
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
normalizeWindows(std::string_view path) noexcept
{
    return normalizeImpl<PathIteratorWindows>(
        path,
        joinWindowsInternal,
        isRootDirectoryNameWindows,
        '\\');
}

[[nodiscard]] std::string
normalizeUnix(std::string_view path) noexcept
{
    return normalizeImpl<PathIteratorUnix>(
        path,
        joinUnixInternal,
        isRootDirectoryNameUnix,
        '/');
}

[[nodiscard]] std::string
normalize(std::string_view path) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    return normalizeWindows(path);
#else
    return normalizeUnix(path);
#endif
}

[[nodiscard]] std::string
toSlash(std::string_view path) noexcept
{
    return strings::replaceAll(path, "\\", "/");
}

[[nodiscard]] std::string
relativeWindows(std::string_view path, std::string_view start) noexcept
{
    const auto fullPath = normalizeWindows(path);
    const auto fullPathStart = normalizeWindows(start);
    return relativeImpl<PathIteratorWindows>(fullPath, fullPathStart, joinWindowsInternal);
}

[[nodiscard]] std::string
relativeUnix(std::string_view path, std::string_view start) noexcept
{
    const auto fullPath = normalizeUnix(path);
    const auto fullPathStart = normalizeUnix(start);
    return relativeImpl<PathIteratorUnix>(fullPath, fullPathStart, joinUnixInternal);
}

[[nodiscard]] std::string
relative(std::string_view path, std::string_view start) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    return relativeWindows(path, start);
#else
    return relativeUnix(path, start);
#endif
}

[[nodiscard]] bool
isAbsoluteWindows(std::string_view path) noexcept
{
    // NOTE: See https://msdn.microsoft.com/en-us/library/bb773660.aspx
    if (path.empty()) {
        return false;
    }
    // NOTE: UNC paths: "\\\\" or "//"
    if (strings::hasPrefix(path, "\\\\") || strings::hasPrefix(path, "//")) {
        return true;
    }
    // NOTE: Check for drive letter (e.g., C:, D:)
    if (auto iter = PathIteratorWindows::begin(path); iter != PathIteratorWindows::end(path)) {
        auto first = *iter;
        // NOTE: Single "/" or "\\" is not absolute on Windows
        if (first == "/" || first == "\\") {
            return false;
        }
        return isRootDirectoryNameWindows(first);
    }
    return false;
}

[[nodiscard]] bool
isAbsoluteUnix(std::string_view path) noexcept
{
    if (path.empty()) {
        return false;
    }
    if (path.front() == '/') {
        return true;
    }
    return false;
}

[[nodiscard]] bool
isAbsolute(std::string_view path) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    return isAbsoluteWindows(path);
#else
    return isAbsoluteUnix(path);
#endif
}

} // namespace pomdog::filepaths
