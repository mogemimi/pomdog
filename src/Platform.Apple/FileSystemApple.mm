// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/FileSystem.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <Foundation/Foundation.h>
#include <unistd.h>
#include <sys/stat.h>
#include <utility>

namespace Pomdog {
namespace {

NSString* ToNSString(const std::string& s)
{
    return [NSString stringWithUTF8String:s.c_str()];
}

} // unnamed namespace

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

bool FileSystem::CreateDirectory(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL result = [[NSFileManager defaultManager]
        createDirectoryAtPath: ToNSString(path)
        withIntermediateDirectories:YES
        attributes:nil
        error:nil];
    return (result == YES);
}

bool FileSystem::CreateDirectories(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL result = [[NSFileManager defaultManager]
        createDirectoryAtPath: ToNSString(path)
        withIntermediateDirectories:YES
        attributes:nil
        error:nil];
    return (result == YES);
}

bool FileSystem::Exists(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath: ToNSString(path)];
    return (exists == YES);
}

bool FileSystem::IsDirectory(const std::string& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL isDirectory = NO;
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath: ToNSString(path) isDirectory:&isDirectory];
    return (exists == YES) && (isDirectory == YES);
}

std::string FileSystem::GetLocalAppDataDirectoryPath()
{
    return GetAppDataDirectoryPath();
}

std::string FileSystem::GetAppDataDirectoryPath()
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* bundleId = [[NSBundle mainBundle] bundleIdentifier];
    NSArray* urlPaths = [fileManager
        URLsForDirectory:NSApplicationSupportDirectory
        inDomains:NSUserDomainMask];

//    NSArray* paths = NSSearchPathForDirectoriesInDomains(
//        NSApplicationSupportDirectory, NSUserDomainMask, YES);

    NSURL* appDirectory = [[urlPaths objectAtIndex:0]
        URLByAppendingPathComponent:bundleId isDirectory:YES];

    BOOL exists = [fileManager fileExistsAtPath:[appDirectory path]];

    if (!exists) {
        // Create app directory
        exists = [fileManager createDirectoryAtURL:appDirectory
            withIntermediateDirectories:NO
            attributes:nil
            error:nil];
    }

    std::string appDataDirecotry = [[appDirectory path] UTF8String];

    if (!exists) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "App direcotry does not exists: " + appDataDirecotry);
    }

    return appDataDirecotry;
}

std::string FileSystem::GetResourceDirectoryPath()
{
    std::string resourceDirectory = [[[NSBundle mainBundle] resourcePath] UTF8String];
    return resourceDirectory;
}

std::string FileSystem::GetTempDirectoryPath()
{
    std::string tempDirectory = [NSTemporaryDirectory() UTF8String];
    return tempDirectory;
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
