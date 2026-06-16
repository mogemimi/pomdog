// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/unsafe_ptr.h"

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

/// Represents a loaded TrueType font that rasterizes glyphs on demand.
class POMDOG_EXPORT TrueTypeFont {
public:
    virtual ~TrueTypeFont();

    /// Rasterizes the glyph for codePoint into a caller-owned atlas.
    ///
    /// Returns the glyph metrics on success, or nullopt when the font has no
    /// glyph for codePoint. pixelHeight is the glyph height in pixels. When sdf
    /// is true, the glyph is rendered as a signed distance field padded by
    /// sdfPadding atlas pixels, defaulting to 4 when sdfPadding is nullopt.
    ///
    /// callback is invoked once with the required glyph width and height. It
    /// must set point to the destination position inside the atlas and set
    /// output to the atlas pixel buffer of size textureWidth by textureHeight;
    /// the glyph is then written into that buffer at point.
    [[nodiscard]] virtual std::optional<FontGlyph>
    rasterizeGlyph(
        char32_t codePoint,
        f32 pixelHeight,
        i32 textureWidth,
        i32 textureHeight,
        bool sdf,
        std::optional<i32> sdfPadding,
        const std::function<void(i32 width, i32 height, Point2D& point, unsafe_ptr<u8>& output)>& callback) = 0;
};

/// Loads a TrueType font file from VFS.
[[nodiscard]] POMDOG_EXPORT std::tuple<std::shared_ptr<TrueTypeFont>, std::unique_ptr<Error>>
loadTrueTypeFont(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::string& filePath);

} // namespace pomdog
