// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include <string>
#include <tuple>

namespace Pomdog {

struct POMDOG_EXPORT PathHelper final {
    static std::string Join(const std::string& path1, const std::string& path2);

    static Detail::BinaryFileStream OpenStream(const std::string& path);

    static std::string GetBaseName(const std::string& path);

    static std::string GetDirectoryName(const std::string& path);

    static std::tuple<std::string, std::string> Split(const std::string& path);

    static std::tuple<std::string, std::string> SplitExtension(const std::string& path);

    static std::string Normalize(const std::string& path);

    static std::string Relative(const std::string& path, const std::string& start);
};

} // namespace Pomdog
