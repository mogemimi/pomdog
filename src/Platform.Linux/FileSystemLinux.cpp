// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/FileSystem.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>

namespace Pomdog {

bool FileSystem::CreateDirectory(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    struct stat st;

    if (::stat(path.c_str(), &st) != -1) {
        return false;
    }
    return ::mkdir(path.c_str(), S_IRWXU) == 0;
}

bool FileSystem::CreateDirectories(const std::string& path)
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

bool FileSystem::Exists(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    return ::access(path.c_str(), F_OK) != -1;
}

bool FileSystem::IsDirectory(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    struct stat st;
    if (::stat(path.c_str(), &st) != -1) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

std::tuple<std::size_t, std::shared_ptr<Error>>
FileSystem::GetFileSize(const std::string& path)
{
    struct ::stat st;
    int rc = ::stat(path.data(), &st);

    if (rc != 0) {
        auto errorCode = Detail::ToErrc(errno);
        return std::make_tuple(0, Errors::New(errorCode, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

std::string FileSystem::GetLocalAppDataDirectoryPath()
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::string FileSystem::GetAppDataDirectoryPath()
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::string FileSystem::GetResourceDirectoryPath()
{
    std::array<char, PATH_MAX + 1> buf;
    std::fill(std::begin(buf), std::end(buf), 0);
    auto size = ::readlink("/proc/self/exe", buf.data(), PATH_MAX);
    if (size < 0) {
        // TODO: Add error handling
        return "";
    }

    std::string executablePath{buf.data(), static_cast<std::size_t>(size)};
    return PathHelper::GetDirectoryName(executablePath);
}

std::string FileSystem::GetTempDirectoryPath()
{
    return P_tmpdir;
}

std::string FileSystem::GetCurrentWorkingDirectory()
{
    char directory[PATH_MAX];
    if (::getcwd(directory, sizeof(directory)) != nullptr) {
        return directory;
    }
    return {};
}

} // namespace Pomdog
