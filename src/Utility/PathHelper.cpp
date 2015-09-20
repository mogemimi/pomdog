// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/Utility/PathHelper.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
std::string PathHelper::Join(std::string const& path1, std::string const& path2)
{
    std::string result = path1;

#if defined(POMDOG_PLATFORM_WIN32)
    if (!result.empty() && '\\' == result.back()) {
        result.erase(std::prev(std::end(result)));
        result += '/';
    }
#endif

    if (!result.empty() && '/' != result.back()) {
        if (!path2.empty() && '/' != path2.front()) {
            result += '/';
        }
    }

    result += path2;
    return std::move(result);
}
//-----------------------------------------------------------------------
BinaryFileStream PathHelper::OpenStream(std::string const& path)
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

    BinaryFileStream result;
    result.Stream = std::move(stream);
    result.SizeInBytes = fileSize;
    return std::move(result);
}
//-----------------------------------------------------------------------
std::string PathHelper::GetBaseName(std::string const& path)
{
    const auto lastIndex = path.find_last_of('/');
    if (std::string::npos != lastIndex) {
        return path.substr(0, lastIndex + 1);
    }
    return path;
}
//-----------------------------------------------------------------------
std::string PathHelper::GetDirectoryName(std::string const& path)
{
    if (!path.empty() && path.back() == '/') {
        return path;
    }

    const auto lastIndex = path.find_last_of('/');
    if (std::string::npos != lastIndex) {
        return path.substr(0, lastIndex);
    }
    return "";
}
//-----------------------------------------------------------------------
std::tuple<std::string, std::string> PathHelper::Split(std::string const& path)
{
    std::tuple<std::string, std::string> result;
    auto lastIndex = path.find_last_of('/');
    if (std::string::npos != lastIndex) {
        std::get<0>(result) = path.substr(0, lastIndex);
        std::get<1>(result) = path.substr(lastIndex + 1);
    }
    else {
        std::get<0>(result) = path;
    }
    return std::move(result);
}
//-----------------------------------------------------------------------
std::tuple<std::string, std::string> PathHelper::SplitExtension(std::string const& path)
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
    return std::move(result);
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
