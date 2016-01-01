// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include <string>
#include <tuple>

namespace Pomdog {

struct POMDOG_EXPORT PathHelper {
    static std::string Join(std::string const& path1, std::string const& path2);

    static Detail::BinaryFileStream OpenStream(std::string const& path);

    static std::string GetBaseName(std::string const& path);

    static std::string GetDirectoryName(std::string const& path);

    static std::tuple<std::string, std::string> Split(std::string const& path);

    static std::tuple<std::string, std::string> SplitExtension(std::string const& path);

    static std::string Normalize(const std::string& path);

    static std::string Relative(const std::string& path, const std::string& start);
};

} // namespace Pomdog
