// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Point2D;
class Error;
struct FontGlyph;
} // namespace pomdog

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog {

class POMDOG_EXPORT TrueTypeFont {
public:
    virtual ~TrueTypeFont();

    [[nodiscard]] virtual std::optional<FontGlyph>
    rasterizeGlyph(
        char32_t codePoint,
        float pixelHeight,
        int textureWidth,
        int textureHeight,
        bool sdf,
        const std::function<void(int width, int height, Point2D& point, std::uint8_t*& output)>& callback) = 0;
};

/// Loads a TrueType font file from VFS.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<TrueTypeFont>, std::unique_ptr<Error>>
loadTrueTypeFont(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath);

} // namespace pomdog
