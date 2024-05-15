// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/apple/file_system_apple.h"
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

[[nodiscard]] std::unique_ptr<Error>
createNewDirectory(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    auto url = [NSURL URLWithString:toNSString(path)];
    if (url == nullptr) {
        return errors::make("NSURL is nullptr");
    }

    NSError* error = nullptr;

    BOOL result = [[NSFileManager defaultManager] createDirectoryAtURL:static_cast<NSURL* _Nonnull>(url)
                                           withIntermediateDirectories:NO
                                                            attributes:nullptr
                                                                 error:&error];

    if (error != nullptr) {
        return errors::make([[error domain] UTF8String]);
    }
    if (result != YES) {
        return errors::make("createDirectoryAtURL() failed");
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
createDirectories(const std::string& path) noexcept
{
    POMDOG_ASSERT(!path.empty());
    auto url = [NSURL URLWithString:toNSString(path)];
    if (url == nullptr) {
        return errors::make("NSURL is nullptr");
    }

    NSError* error = nullptr;

    BOOL result = [[NSFileManager defaultManager] createDirectoryAtURL:static_cast<NSURL* _Nonnull>(url)
                                           withIntermediateDirectories:YES
                                                            attributes:nullptr
                                                                 error:&error];

    if (error != nullptr) {
        return errors::make([[error domain] UTF8String]);
    }
    if (result != YES) {
        return errors::make("createDirectoryAtURL() failed");
    }
    return nullptr;
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
        const auto err = detail::toErrc(errno);
        return std::make_tuple(0, errors::makeIOError(err, "::stat() failed"));
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

    NSArray* urlPaths = [fileManager
        URLsForDirectory:NSApplicationSupportDirectory
               inDomains:NSUserDomainMask];

    if ((urlPaths == nullptr) || (urlPaths.count <= 0)) {
        return std::make_tuple(std::string{}, errors::make("urlPaths is empty"));
    }

    // FIXME:
    // NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);

    NSURL* appDirectory = [[urlPaths objectAtIndex:0]
        URLByAppendingPathComponent:bundleID
                        isDirectory:YES];

    NSString* path = [appDirectory path];
    if (path == nullptr) {
        return std::make_tuple(std::string{}, errors::make("[appDirectory path] is nullptr"));
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
        return std::make_tuple(std::string{}, errors::make("direcotry" + appDataDirecotry + "does not exist"));
    }

    return std::make_tuple(std::move(appDataDirecotry), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getResourceDirectoryPath() noexcept
{
    std::string resourceDirectory = [[[NSBundle mainBundle] resourcePath] UTF8String];
    return std::make_tuple(std::move(resourceDirectory), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getTempDirectoryPath() noexcept
{
    std::string tempDirectory = [NSTemporaryDirectory() UTF8String];
    return std::make_tuple(std::move(tempDirectory), nullptr);
}

[[nodiscard]] std::tuple<std::string, std::unique_ptr<Error>>
getCurrentWorkingDirectory() noexcept
{
    char dir[PATH_MAX];
    if (::getcwd(dir, sizeof(dir)) == nullptr) {
        const auto err = detail::toErrc(errno);
        return std::make_tuple("", errors::makeIOError(err, "::getcwd() failed"));
    }
    return std::make_tuple(dir, nullptr);
}

} // namespace pomdog::detail::apple
