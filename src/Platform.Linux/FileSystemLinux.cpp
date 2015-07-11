// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Utility/detail/FileSystem.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectory(std::string const& path)
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
bool FileSystem::CreateDirectories(std::string const& path)
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
bool FileSystem::Exists(std::string const& path)
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
bool FileSystem::IsDirectory(std::string const& path)
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
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
std::string FileSystem::GetTempDirectoryPath()
{
    ///@todo Not implemented
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
