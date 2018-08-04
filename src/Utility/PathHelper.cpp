// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include <algorithm>
#include <regex>
#include <utility>
#include <vector>

namespace Pomdog {
namespace {

std::string::size_type findFirstOfSlash(
    const std::string& path,
    std::string::size_type first = 0)
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

bool isRootDirectoryName(const std::string& name)
{
    if (name == "/") {
        return true;
    }
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    // NOTE: Windows's drive (ex. 'C:' drive)
    std::regex windowsDriveRegex(R"(^[A-Za-z]:)");
    if (std::regex_match(name, windowsDriveRegex)) {
        return true;
    }
    // TODO: Add windows's '\\[server name]' support.
#endif
    return false;
}

class PathIterator {
    const std::string* source;
    std::string::size_type startPos;
    std::string::size_type endPos;

public:
    PathIterator()
        : source(nullptr)
        , startPos(std::string::npos)
        , endPos(std::string::npos)
    {
    }

    PathIterator(const std::string& path_, std::string::size_type start_, std::string::size_type end_)
        : source(&path_)
        , startPos(start_)
        , endPos(end_)
    {
    }

    std::string operator*() const
    {
        POMDOG_ASSERT(source != nullptr);
        if (source->empty()) {
            return "";
        }
        POMDOG_ASSERT(startPos != std::string::npos);
        POMDOG_ASSERT(endPos != std::string::npos);
        if (endPos == std::string::npos) {
            return source->substr(startPos);
        }
        return source->substr(startPos, endPos - startPos);
    }

    bool operator==(const PathIterator& iter) const
    {
        return (source == iter.source)
            && (startPos == iter.startPos)
            && (endPos == iter.endPos);
    }

    bool operator!=(const PathIterator& iter) const
    {
        return (source != iter.source)
            || (startPos != iter.startPos)
            || (endPos != iter.endPos);
    }

    static PathIterator begin(const std::string& path)
    {
        if (path.empty()) {
            return PathIterator(path, std::string::npos, std::string::npos);
        }
        auto index = findFirstOfSlash(path);
        if (std::string::npos == index) {
            return PathIterator(path, 0, path.size());
        }
        return PathIterator(path, 0, index);
    }

    static PathIterator end(const std::string& path)
    {
        return PathIterator(path, std::string::npos, std::string::npos);
    }

    static PathIterator next(const PathIterator& iter)
    {
        POMDOG_ASSERT(iter.source != nullptr);
        POMDOG_ASSERT(iter.startPos != std::string::npos);
        POMDOG_ASSERT(iter.endPos != std::string::npos);
        auto endPos = iter.endPos;
        if (endPos == iter.source->size()) {
            return PathIterator(*iter.source, std::string::npos, std::string::npos);
        }

        if (iter.source->at(endPos) == '/'
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
            || iter.source->at(endPos) == '\\'
#endif
        ) {
            if (endPos < iter.source->size()) {
                endPos++;
            }
        }
        auto index = findFirstOfSlash(*iter.source, endPos);
        if (std::string::npos == index) {
            if (endPos < iter.source->size()) {
                return PathIterator(*iter.source, endPos, iter.source->size());
            }
            return PathIterator(*iter.source, index, index);
        }
        return PathIterator(*iter.source, endPos, index);
    }
};

#if 0
TEST(PathIterator, Case_01)
{
    std::string path = "";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
    EXPECT_EQ("", *iter);
}

TEST(PathIterator, Case_02)
{
    std::string path = "/";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("/", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}

TEST(PathIterator, Case_03)
{
    std::string path = ".";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ(".", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}

TEST(PathIterator, Case_04)
{
    std::string path = "../hoge";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("..", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("hoge", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}

TEST(PathIterator, Case_05)
{
    std::string path = "../hoge/";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("..", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("hoge", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}

TEST(PathIterator, Case_06)
{
    std::string path = "../foo\\hoge";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("..", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("foo", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("hoge", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}

TEST(PathIterator, Case_07)
{
    std::string path = "../foo\\hoge\\";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("..", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("foo", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ("hoge", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}

TEST(PathIterator, Case_08)
{
    std::string path = ".\\";
    auto iter = PathIterator::begin(path);
    EXPECT_EQ(iter, PathIterator::begin(path));
    EXPECT_NE(iter, PathIterator::end(path));
    EXPECT_EQ(".", *iter);
    iter = PathIterator::next(iter);
    EXPECT_NE(iter, PathIterator::begin(path));
    EXPECT_EQ(iter, PathIterator::end(path));
}
#endif

} // unnamed namespace

std::string PathHelper::Join(const std::string& path1, const std::string& path2)
{
    std::string result = path1;
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

Detail::BinaryFileStream PathHelper::OpenStream(const std::string& path)
{
    std::ifstream stream(path, std::ios::ate | std::ios::binary);
    std::size_t fileSize = 0;

    if (stream.is_open()) {
        const auto pos = stream.tellg();
        if (pos > 0) {
            fileSize = static_cast<std::size_t>(pos);
        }
        stream.clear();
        stream.seekg(0, stream.beg);
    }

    Detail::BinaryFileStream result;
    result.Stream = std::move(stream);
    result.SizeInBytes = fileSize;
    return result;
}

std::string PathHelper::GetBaseName(const std::string& path)
{
    const auto lastIndex = path.find_last_of('/');
    if (std::string::npos != lastIndex) {
        return path.substr(lastIndex + 1);
    }
    return path;
}

std::string PathHelper::GetDirectoryName(const std::string& path)
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

std::tuple<std::string, std::string> PathHelper::Split(const std::string& path)
{
    std::tuple<std::string, std::string> result;
    auto lastIndex = path.find_last_of('/');
    if (std::string::npos != lastIndex) {
        std::get<0>(result) = path.substr(0, lastIndex);
        std::get<1>(result) = path.substr(lastIndex + 1);
    }
    else {
        std::get<1>(result) = path;
    }
    return result;
}

std::tuple<std::string, std::string> PathHelper::SplitExtension(const std::string& path)
{
    std::tuple<std::string, std::string> result;
    auto lastIndex = path.find_last_of('.');
    if (std::string::npos != lastIndex) {
        std::get<0>(result) = path.substr(0, lastIndex);
        std::get<1>(result) = path.substr(lastIndex + 1);
    }
    else {
        std::get<0>(result) = path;
    }
    return result;
}

std::string PathHelper::Normalize(const std::string& path)
{
    const auto isFullPath = [&path]() -> bool {
        if (path.empty()) {
            return false;
        }
        if (path.front() == '/') {
            return true;
        }
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        auto iter = PathIterator::begin(path);
        if (iter != PathIterator::end(path)) {
            return isRootDirectoryName(*iter);
        }
#endif
        return false;
    }();

    auto fullPath = path;
    if (!isFullPath) {
        // NOTE: 'path' is not full path.
        fullPath = PathHelper::Join(
            FileSystem::GetCurrentWorkingDirectory(), fullPath);
    }

    std::vector<std::string> paths;
    auto iter = PathIterator::begin(fullPath);
    while (iter != PathIterator::end(fullPath)) {
        auto current = *iter;
        iter = PathIterator::next(iter);

        if (current == ".") {
            continue;
        }
        if (current == "..") {
            assert(!paths.empty());
            if (paths.empty()) {
                return fullPath;
            }
            paths.pop_back();
            continue;
        }
        paths.push_back(current);
    }

    fullPath.clear();
    for (auto & current : paths) {
        if (fullPath.empty() && isRootDirectoryName(current)) {
            fullPath = current;
            continue;
        }
        fullPath = PathHelper::Join(fullPath, current);
    }
    return fullPath;
}

std::string
PathHelper::Relative(const std::string& path, const std::string& start)
{
    const auto fullPath = PathHelper::Normalize(path);
    const auto fullPathStart = PathHelper::Normalize(start);

    auto iterL = PathIterator::begin(fullPath);
    auto iterR = PathIterator::begin(fullPathStart);
    while ((iterL != PathIterator::end(fullPath))
        && (iterR != PathIterator::end(fullPathStart))) {
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

} // namespace Pomdog
