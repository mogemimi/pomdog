// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/detail/FileSystem.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Content/Utility/PathHelper.hpp"
#include <array>
#include <algorithm>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectory(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    struct stat st;

    if (::stat(path.c_str(), &st) != -1) {
        return false;
    }
    return ::mkdir(path.c_str(), S_IRWXU) == 0;
}
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectories(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    if (path.empty()) {
        return false;
    }

    auto tmp = path;
    if (tmp.back() == '/') {
        tmp.pop_back();
    }

    POMDOG_ASSERT(!tmp.empty());
    if (tmp.empty()) {
        return false;
    }

    for (auto iter = std::next(std::begin(tmp), 1); iter != std::end(tmp); iter++) {
        if (*iter == '/') {
            *iter = 0;
            ::mkdir(tmp.c_str(), S_IRWXU);
            *iter = '/';
        }
    }
    return ::mkdir(tmp.c_str(), S_IRWXU) == 0;
}
//-----------------------------------------------------------------------
bool FileSystem::Exists(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    return ::access(path.c_str(), F_OK) != -1;
}
//-----------------------------------------------------------------------
bool FileSystem::IsDirectory(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    struct stat st;
    if (::stat(path.c_str(), &st) != -1) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetLocalAppDataDirectoryPath()
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::string FileSystem::GetAppDataDirectoryPath()
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::string FileSystem::GetResourceDirectoryPath()
{
    std::array<char, PATH_MAX + 1> buf;
    std::fill(std::begin(buf), std::end(buf), 0);
    ::readlink("/proc/self/exe", buf.data(), PATH_MAX);

    std::string executablePath = buf.data();
    return PathHelper::GetDirectoryName(executablePath);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetTempDirectoryPath()
{
    return P_tmpdir;
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
