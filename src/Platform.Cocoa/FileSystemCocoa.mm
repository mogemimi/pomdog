// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Utility/detail/FileSystem.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <Foundation/Foundation.h>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace {

static NSString* ToNSString(std::string const& s)
{
    return [NSString stringWithUTF8String:s.c_str()];
}

} // unnamed namespace
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectory(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL result = [[NSFileManager defaultManager]
        createDirectoryAtPath: ToNSString(path)
        withIntermediateDirectories:YES
        attributes:nil
        error:nil];
    return (result == YES);
}
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectories(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL result = [[NSFileManager defaultManager]
        createDirectoryAtPath: ToNSString(path)
        withIntermediateDirectories:YES
        attributes:nil
        error:nil];
    return (result == YES);
}
//-----------------------------------------------------------------------
bool FileSystem::Exists(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath: ToNSString(path)];
    return (exists == YES);
}
//-----------------------------------------------------------------------
bool FileSystem::IsDirectory(std::string const& path)
{
    POMDOG_ASSERT(!path.empty());
    BOOL isDirectory = NO;
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath: ToNSString(path) isDirectory:&isDirectory];
    return (exists == YES) && (isDirectory == YES);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetLocalAppDataDirectoryPath()
{
    return GetAppDataDirectoryPath();
}
//-----------------------------------------------------------------------
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

    return std::move(appDataDirecotry);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetResourceDirectoryPath()
{
    std::string resourceDirectory = [[[NSBundle mainBundle] resourcePath] UTF8String];
    return std::move(resourceDirectory);
}
//-----------------------------------------------------------------------
std::string FileSystem::GetTempDirectoryPath()
{
    std::string tempDirectory = [NSTemporaryDirectory() UTF8String];
    return std::move(tempDirectory);
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
