// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include <string>
#include <tuple>

namespace Pomdog {

struct POMDOG_EXPORT PathHelper final {
    /// Joins two paths into a single path, adding a separator if necessary.
    [[nodiscard]] static std::string
    Join(const std::string& path1, const std::string& path2);

    static Detail::BinaryFileStream OpenStream(const std::string& path);

    /// Returns the last element of path.
    [[nodiscard]] static std::string
    GetBaseName(const std::string& path);

    /// Returns the directory name of path.
    [[nodiscard]] static std::string
    GetDirectoryName(const std::string& path);

    /// Returns the directory name of path and the last element of path.
    [[nodiscard]] static std::tuple<std::string, std::string>
    Split(const std::string& path);

    /// Returns the base name of path and file name extension.
    [[nodiscard]] static std::tuple<std::string, std::string>
    SplitExtension(const std::string& path);

    /// Returns the shortest path.
    [[nodiscard]] static std::string
    Normalize(const std::string& path);

    /// Returns a relative path from the start directory.
    [[nodiscard]] static std::string
    Relative(const std::string& path, const std::string& start);
};

} // namespace Pomdog
