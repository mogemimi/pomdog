// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/apple/file_system_apple.h"
#include "pomdog/filesystem/posix/file_system_posix.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <Foundation/Foundation.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::apple {
namespace {

[[nodiscard]] NSString*
toNSString(const std::string& s) noexcept
{
    return [NSString stringWithUTF8String:s.data()];
}

} // namespace

[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectory(const std::string& path) noexcept
{
    // NOTE: Create directories with 0777, which the umask masks, as NSFileManager does.
    return posix::createDirectory(path, S_IRWXU | S_IRWXG | S_IRWXO);
}

[[nodiscard]] std::tuple<platformfs::DirectoryCreation, std::unique_ptr<Error>>
createDirectories(const std::string& path) noexcept
{
    return posix::createDirectories(path, S_IRWXU | S_IRWXG | S_IRWXO);
}

[[nodiscard]] bool
exists(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath:toNSString(path)];
    return (exists == YES);
}

[[nodiscard]] bool
isDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    BOOL isDirectory = NO;
    BOOL exists = [[NSFileManager defaultManager]
        fileExistsAtPath:toNSString(path)
             isDirectory:&isDirectory];
    return (exists == YES) && (isDirectory == YES);
}

[[nodiscard]] std::tuple<std::size_t, std::unique_ptr<Error>>
getFileSize(const std::string& path) noexcept
{
    struct ::stat st;
    if (::stat(path.data(), &st) != 0) {
        const auto err = errors::toErrc(errno);
        return std::make_tuple(0, errors::fromErrc(err, "::stat() failed"));
    }
    return std::make_tuple(st.st_size, nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getLocalAppDataDirectoryPath() noexcept
{
    return getAppDataDirectoryPath();
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getAppDataDirectoryPath() noexcept
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
    if (bundleID == nullptr) {
        return std::make_tuple(std::string{}, errors::make("bundleID is nullptr"));
    }

    // NOTE: Keep the element type in the declaration so that the compiler rejects
    // a mix-up with `NSSearchPathForDirectoriesInDomains()`, which returns NSString paths.
    NSArray<NSURL*>* urlPaths = [fileManager
        URLsForDirectory:NSApplicationSupportDirectory
               inDomains:NSUserDomainMask];

    NSURL* applicationSupportDirectory = urlPaths.firstObject;
    if (applicationSupportDirectory == nullptr) {
        return std::make_tuple(std::string{}, errors::make("urlPaths is empty"));
    }

    NSURL* appDirectory = [applicationSupportDirectory
        URLByAppendingPathComponent:bundleID
                        isDirectory:YES];

    NSString* path = [appDirectory path];
    if (path == nullptr) {
        return std::make_tuple(std::string{}, errors::make("[appDirectory path] is nullptr"));
    }

    const char* appDataDirectoryUTF8 = [path UTF8String];
    if (appDataDirectoryUTF8 == nullptr) {
        return std::make_tuple(std::string{}, errors::make("[path UTF8String] is nullptr"));
    }

    std::string appDataDirectory = appDataDirectoryUTF8;

    BOOL isDirectory = NO;
    if ([fileManager fileExistsAtPath:path isDirectory:&isDirectory]) {
        if (!isDirectory) {
            return std::make_tuple(std::string{}, errors::make("'" + appDataDirectory + "' exists but is not a directory"));
        }
        return std::make_tuple(std::move(appDataDirectory), nullptr);
    }

    // NOTE: Create the app directory along with any missing parent such as Application Support.
    NSError* createError = nullptr;
    const BOOL created = [fileManager createDirectoryAtURL:appDirectory
                               withIntermediateDirectories:YES
                                                attributes:nullptr
                                                     error:&createError];
    if (!created) {
        std::string message = "failed to create directory '" + appDataDirectory + "'";
        if (createError != nullptr) {
            if (const char* description = [[createError localizedDescription] UTF8String]; description != nullptr) {
                message += ": ";
                message += description;
            }
        }
        return std::make_tuple(std::string{}, errors::make(std::move(message)));
    }

    return std::make_tuple(std::move(appDataDirectory), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept
{
    const char* resourceDirectory = [[[NSBundle mainBundle] resourcePath] UTF8String];
    if (resourceDirectory == nullptr) {
        return std::make_tuple(std::string{}, errors::make("[[[NSBundle mainBundle] resourcePath] UTF8String] is nullptr"));
    }
    return std::make_tuple(std::string{resourceDirectory}, nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept
{
    const char* tempDirectory = [NSTemporaryDirectory() UTF8String];
    if (tempDirectory == nullptr) {
        return std::make_tuple(std::string{}, errors::make("[NSTemporaryDirectory() UTF8String] is nullptr"));
    }
    return std::make_tuple(std::string{tempDirectory}, nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept
{
    char dir[PATH_MAX];
    if (::getcwd(dir, sizeof(dir)) == nullptr) {
        const auto err = errors::toErrc(errno);
        return std::make_tuple("", errors::fromErrc(err, "::getcwd() failed"));
    }
    return std::make_tuple(dir, nullptr);
}

} // namespace pomdog::detail::apple
