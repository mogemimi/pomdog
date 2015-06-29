// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/Utility/PathHelper.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
std::string PathHelper::Join(std::string const& path1, std::string const& path2)
{
    std::string result = path1;

    if (!path1.empty() && '/' != path1.back()) {
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
} // namespace Detail
} // namespace Pomdog
