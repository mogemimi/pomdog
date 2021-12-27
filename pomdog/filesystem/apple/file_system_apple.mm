// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/apple/file_system_apple.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"
#include <Foundation/Foundation.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>

namespace pomdog::detail::apple {
namespace {

[[nodiscard]] NSString*
ToNSString(const std::string& s) noexcept
{
    return [NSString stringWithUTF8String:s.data()];
}

} // namespace

std::unique_ptr<Error>
CreateNewDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    auto url = [NSURL URLWithString:ToNSString(path)];

    NSError* error = nullptr;

    BOOL result = [[NSFileManager defaultManager] createDirectoryAtURL:url
                                           withIntermediateDirectories:NO
                                                            attributes:nullptr
                                                                 error:&error];

    if (error != nullptr) {
        return errors::New([[error domain] UTF8String]);
    }
    if (result != YES) {
        return errors::New("createDirectoryAtURL() failed");
    }
    return nullptr;
}

std::unique_ptr<Error>
CreateDirectories(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    auto url = [NSURL URLWithString:ToNSString(path)];

    NSError* error = nullptr;

    BOOL result = [[NSFileManager defaultManager] createDirectoryAtURL:url
                                           withIntermediateDirectories:YES
                                                            attributes:nullptr
                                                                 error:&error];

    if (error != nullptr) {
        return errors::New([[error domain] UTF8String]);
    }
    if (result != YES) {
        return errors::New("createDirectoryAtURL() failed");
    }
    return nullptr;
}

bool Exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath:ToNSString(path)];
    return (exists == YES);
}

bool IsDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    BOOL isDirectory = NO;
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath:ToNSString(path)
             isDirectory:&isDirectory];
    return (exists == YES) && (isDirectory == YES);
}

std::tuple<std::size_t, std::unique_ptr<Error>>
GetFileSize(const std::string& path) noexcept
{
    struct ::stat st;
    if (::stat(path.data(), &st) != 0) {
        auto err = detail::ToErrc(errno);
        return std::make_tuple(0, errors::New(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetLocalAppDataDirectoryPath() noexcept
{
    return GetAppDataDirectoryPath();
}

std::tuple<std::string, std::unique_ptr<Error>>
GetAppDataDirectoryPath() noexcept
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
    if (bundleID == nullptr) {
        return std::make_tuple("", errors::New("bundleID is nullptr"));
    }

    NSArray* urlPaths = [fileManager
        URLsForDirectory:NSApplicationSupportDirectory
               inDomains:NSUserDomainMask];

    if ((urlPaths == nullptr) || (urlPaths.count <= 0)) {
        return std::make_tuple("", errors::New("urlPaths is empty"));
    }

    // FIXME:
    // NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);

    NSURL* appDirectory = [[urlPaths objectAtIndex:0]
        URLByAppendingPathComponent:bundleID
                        isDirectory:YES];

    NSString* path = [appDirectory path];
    if (path == nullptr) {
        return std::make_tuple("", errors::New("[appDirectory path] is nullptr"));
    }

    BOOL exists = [fileManager fileExistsAtPath:path];
    if (!exists) {
        // NOTE: Create app directory
        exists = [fileManager createDirectoryAtURL:appDirectory
                       withIntermediateDirectories:NO
                                        attributes:nullptr
                                             error:nullptr];
    }

    std::string appDataDirecotry = [[appDirectory path] UTF8String];
    if (!exists) {
        return std::make_tuple("", errors::New("direcotry" + appDataDirecotry + "does not exist"));
    }

    return std::make_tuple(std::move(appDataDirecotry), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetResourceDirectoryPath() noexcept
{
    std::string resourceDirectory = [[[NSBundle mainBundle] resourcePath] UTF8String];
    return std::make_tuple(std::move(resourceDirectory), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetTempDirectoryPath() noexcept
{
    std::string tempDirectory = [NSTemporaryDirectory() UTF8String];
    return std::make_tuple(std::move(tempDirectory), nullptr);
}

std::tuple<std::string, std::unique_ptr<Error>>
GetCurrentWorkingDirectory() noexcept
{
    char dir[PATH_MAX];
    if (::getcwd(dir, sizeof(dir)) == nullptr) {
        auto err = detail::ToErrc(errno);
        return std::make_tuple("", errors::New(err, "::getcwd() failed"));
    }
    return std::make_tuple(dir, nullptr);
}

} // namespace pomdog::detail::apple
