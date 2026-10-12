// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/renderer.h"
#include "pomdog/experimental/devgui/antialiasing.h"
#include "pomdog/experimental/devgui/draw_list.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/rasterizer_desc.h"
#include "pomdog/gpu/sampler_desc.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/graphics/sprite_batch.h"
#include "pomdog/graphics/sprite_font.h"
#include "pomdog/graphics/sprite_font_sdf.h"
#include "pomdog/math/color.h"
#include "pomdog/math/math_constants.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The distance from the top of a text line to its baseline, as a fraction of
/// the font size. It centers both capitals and lowercase letters in a line box
/// that is one font size tall.
constexpr f32 baselineRatio = 0.8f;

[[nodiscard]] Rect2D
toPhysicalRect(const F32Rect2D& rect, f32 pixelRatio) noexcept
{
    const auto left = static_cast<i32>(std::floor(rect.x * pixelRatio));
    const auto top = static_cast<i32>(std::floor(rect.y * pixelRatio));
    const auto right = static_cast<i32>(std::ceil((rect.x + rect.width) * pixelRatio));
    const auto bottom = static_cast<i32>(std::ceil((rect.y + rect.height) * pixelRatio));
    return Rect2D{left, top, std::max(0, right - left), std::max(0, bottom - top)};
}

struct Contour final {
    std::array<Vector2, detail::contourCapacity> points;
    std::size_t count = 0;

    void append(const Vector2& point) noexcept
    {
        if ((count == 0) || (math::distanceSquared(points[count - 1], point) > 0.000001f)) {
            points[count++] = point;
        }
    }

    [[nodiscard]] std::span<const Vector2> getPoints() noexcept
    {
        if ((count > 1) && (math::distanceSquared(points[0], points[count - 1]) <= 0.000001f)) {
            --count;
        }
        return std::span<const Vector2>{points}.first(count);
    }
};

void fillAntialiasedRoundedRect(
    PrimitiveBatch& batch,
    f32 left, f32 bottom, f32 width, f32 height, f32 radius,
    const Color& color, f32 fringeWidth, f32 pixelRatio)
{
    detail::drawAntialiasedRoundedRect(left, bottom, width, height, radius, color, fringeWidth, pixelRatio,
        [&batch](const Vector2& a, const Vector2& b, const Vector2& c, const Color& ca, const Color& cb, const Color& cc) {
            batch.drawTriangle(a, b, c, ca, cb, cc);
        });
}

void strokeAntialiasedRoundedRect(
    PrimitiveBatch& batch,
    f32 left, f32 bottom, f32 width, f32 height, f32 radius, f32 thickness,
    const Color& color, f32 fringeWidth, f32 pixelRatio)
{
    const auto t = std::clamp(thickness, 0.0f, std::min(width, height) * 0.5f);
    if (t <= 0.0f) {
        return;
    }
    if (t >= std::min(width, height) * 0.5f) {
        fillAntialiasedRoundedRect(batch, left, bottom, width, height, radius, color, fringeWidth, pixelRatio);
        return;
    }
    const auto r = std::clamp(radius, 0.0f, std::min(width, height) * 0.5f);
    const auto segments = detail::quarterCircleSegments(r, pixelRatio);
    std::array<std::array<Vector2, detail::contourCapacity>, 4> contours;
    std::array<f32, 4> offsets{{fringeWidth * 0.5f, -fringeWidth * 0.5f, -t + fringeWidth * 0.5f, -t - fringeWidth * 0.5f}};
    auto solid = color;
    if (t < fringeWidth) {
        solid.a = static_cast<u8>(std::round(static_cast<f32>(color.a) * t / fringeWidth));
        offsets = {{-t * 0.5f + fringeWidth, -t * 0.5f, -t * 0.5f, -t * 0.5f - fringeWidth}};
    }
    auto transparent = solid;
    transparent.a = 0;
    std::size_t count = 0;
    for (i32 corner = 0; corner < 4; ++corner) {
        for (i32 i = 0; i <= segments; ++i) {
            const auto angle = math::PiOver2<f32> * (static_cast<f32>(corner) + static_cast<f32>(i) / static_cast<f32>(segments));
            for (std::size_t j = 0; j < contours.size(); ++j) {
                contours[j][count] = detail::roundedContourPoint(left, bottom, width, height, r, corner, angle, offsets[j]);
            }
            ++count;
        }
    }
    const std::array<Color, 4> colors{transparent, solid, solid, transparent};
    for (std::size_t band = 0; band + 1 < contours.size(); ++band) {
        for (std::size_t i = 0; i < count; ++i) {
            const auto next = (i + 1) % count;
            batch.drawTriangle(contours[band][i], contours[band][next], contours[band + 1][next], colors[band], colors[band], colors[band + 1]);
            batch.drawTriangle(contours[band][i], contours[band + 1][next], contours[band + 1][i], colors[band], colors[band + 1], colors[band + 1]);
        }
    }
}

/// Returns the point at angle on the circle of radius around center.
[[nodiscard]] Vector2
pointOnCircle(const Vector2& center, f32 radius, f32 angle) noexcept
{
    return Vector2{center.x + radius * std::cos(angle), center.y + radius * std::sin(angle)};
}

/// Fills the ring between innerRadius and outerRadius from startAngle over a
/// quarter turn. An inner radius of zero fills a quarter circle.
void fillQuarterRing(
    PrimitiveBatch& batch,
    const Vector2& center,
    f32 innerRadius,
    f32 outerRadius,
    f32 startAngle,
    const Color& color)
{
    constexpr f32 pixelRatio = 1.0f;
    const auto segments = detail::quarterCircleSegments(outerRadius, pixelRatio);
    const auto step = math::PiOver2<f32> / static_cast<f32>(segments);
    for (i32 i = 0; i < segments; ++i) {
        const auto a0 = startAngle + step * static_cast<f32>(i);
        const auto a1 = a0 + step;
        const auto outer0 = pointOnCircle(center, outerRadius, a0);
        const auto outer1 = pointOnCircle(center, outerRadius, a1);
        if (innerRadius <= 0.0f) {
            batch.drawTriangle(center, outer0, outer1, color);
            continue;
        }
        const auto inner0 = pointOnCircle(center, innerRadius, a0);
        const auto inner1 = pointOnCircle(center, innerRadius, a1);
        batch.drawTriangle(outer0, outer1, inner1, color);
        batch.drawTriangle(outer0, inner1, inner0, color);
    }
}

/// Fills a rectangle whose corners are rounded by radius. The rectangle is in
/// the y-up space of the batch, with its bottom-left corner at (left, bottom).
void fillRoundedRect(PrimitiveBatch& batch, f32 left, f32 bottom, f32 width, f32 height, f32 radius, const Color& color)
{
    const auto pivot = Vector2{0.0f, 0.0f};
    if (radius < 0.5f) {
        batch.drawRectangle(Vector2{left, bottom}, width, height, pivot, color);
        return;
    }
    const auto r = radius;
    const auto right = left + width;
    const auto top = bottom + height;
    batch.drawRectangle(Vector2{left + r, bottom}, width - r * 2.0f, height, pivot, color);
    batch.drawRectangle(Vector2{left, bottom + r}, r, height - r * 2.0f, pivot, color);
    batch.drawRectangle(Vector2{right - r, bottom + r}, r, height - r * 2.0f, pivot, color);
    fillQuarterRing(batch, Vector2{right - r, top - r}, 0.0f, r, 0.0f, color);
    fillQuarterRing(batch, Vector2{left + r, top - r}, 0.0f, r, math::PiOver2<f32>, color);
    fillQuarterRing(batch, Vector2{left + r, bottom + r}, 0.0f, r, math::Pi<f32>, color);
    fillQuarterRing(batch, Vector2{right - r, bottom + r}, 0.0f, r, math::Pi<f32> * 1.5f, color);
}

/// Outlines a rectangle whose corners are rounded by radius, thickness pixels
/// wide inside it. The rectangle is in the y-up space of the batch.
void strokeRoundedRect(
    PrimitiveBatch& batch,
    f32 left,
    f32 bottom,
    f32 width,
    f32 height,
    f32 radius,
    f32 thickness,
    const Color& color)
{
    const auto pivot = Vector2{0.0f, 0.0f};
    const auto t = std::min({thickness, width * 0.5f, height * 0.5f});
    const auto r = std::max(radius, t);
    const auto right = left + width;
    const auto top = bottom + height;
    batch.drawRectangle(Vector2{left + r, bottom}, width - r * 2.0f, t, pivot, color);
    batch.drawRectangle(Vector2{left + r, top - t}, width - r * 2.0f, t, pivot, color);
    batch.drawRectangle(Vector2{left, bottom + r}, t, height - r * 2.0f, pivot, color);
    batch.drawRectangle(Vector2{right - t, bottom + r}, t, height - r * 2.0f, pivot, color);
    if (radius < 0.5f) {
        // NOTE: Fill the square corners left by the four edges.
        batch.drawRectangle(Vector2{left, bottom}, r, r, pivot, color);
        batch.drawRectangle(Vector2{right - r, bottom}, r, r, pivot, color);
        batch.drawRectangle(Vector2{left, top - r}, r, r, pivot, color);
        batch.drawRectangle(Vector2{right - r, top - r}, r, r, pivot, color);
        return;
    }
    const auto inner = r - t;
    fillQuarterRing(batch, Vector2{right - r, top - r}, inner, r, 0.0f, color);
    fillQuarterRing(batch, Vector2{left + r, top - r}, inner, r, math::PiOver2<f32>, color);
    fillQuarterRing(batch, Vector2{left + r, bottom + r}, inner, r, math::Pi<f32>, color);
    fillQuarterRing(batch, Vector2{right - r, bottom + r}, inner, r, math::Pi<f32> * 1.5f, color);
}

class RendererImpl final : public Renderer {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<vfs::FileSystemContext>& fs,
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        const std::shared_ptr<SpriteFont>& font,
        f32 fontRasterSize) noexcept;

    [[nodiscard]] f32
    measureWidth(std::string_view text, f32 fontSize) override;

    void
    draw(const std::shared_ptr<gpu::CommandList>& commandList, const DrawList& drawList, f32 pixelRatio, f32 antialiasingWidth) override;

private:
    void
    drawShape(const DrawList& drawList, const DrawCommand& command, f32 viewportHeight, f32 fringeWidth, f32 pixelRatio);

    void
    drawText(const DrawList& drawList, const DrawCommand& command, f32 viewportHeight, f32 pixelRatio);

private:
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<SpriteFont> font_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> fontPipeline_;
    f32 fontRasterSize_ = 1.0f;
};

std::unique_ptr<Error>
RendererImpl::initialize(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<SpriteFont>& font,
    f32 fontRasterSize) noexcept
{
    if (font == nullptr) {
        return errors::make("font must not be null");
    }
    if (fontRasterSize <= 0.0f) {
        return errors::make("fontRasterSize must be positive");
    }

    graphicsDevice_ = graphicsDevice;
    font_ = font;
    fontRasterSize_ = fontRasterSize;

    if (auto [p, err] = createPrimitivePipeline(
            fs,
            graphicsDevice,
            gpu::DepthStencilDesc::createNone(),
            gpu::RasterizerDesc::createCullNone());
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice, 32768); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }
    if (auto [p, err] = createSpritePipeline(
            fs,
            graphicsDevice,
            gpu::BlendDesc::createNonPremultiplied(),
            gpu::RasterizerDesc::createCullNone(),
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceField);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        fontPipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice, 4096); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }
    return nullptr;
}

f32 RendererImpl::measureWidth(std::string_view text, f32 fontSize)
{
    if (text.empty()) {
        return 0.0f;
    }
    return font_->measureString(graphicsDevice_, text).x * (fontSize / fontRasterSize_);
}

void RendererImpl::draw(const std::shared_ptr<gpu::CommandList>& commandList, const DrawList& drawList, f32 pixelRatio, f32 antialiasingWidth)
{
    POMDOG_ASSERT(commandList != nullptr);
    POMDOG_ASSERT(std::isfinite(pixelRatio) && (pixelRatio > 0.0f));
    POMDOG_ASSERT(std::isfinite(antialiasingWidth) && (antialiasingWidth >= 0.0f));

    const auto viewportSize = drawList.getViewportSize();
    if ((viewportSize.x <= 0.0f) || (viewportSize.y <= 0.0f)) {
        return;
    }

    // NOTE: Draw lists use a y-down space with the origin at the top-left
    // corner, while the 2D batches use a y-up space. The projection maps the
    // y-up space with its origin at the bottom-left corner, and each command
    // flips its y coordinates.
    const auto projection = Matrix4x4::createOrthographicOffCenterLH(0.0f, viewportSize.x, 0.0f, viewportSize.y, 0.0f, 100.0f);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projection);
    spriteBatch_->reset();
    spriteBatch_->setTransform(projection);

    const auto commands = drawList.getCommands();
    const auto clipRects = drawList.getClipRects();

    for (const auto& range : drawList.getRanges()) {
        auto begin = range.commandBegin;
        while (begin < range.commandEnd) {
            // NOTE: Draw each run of commands that share a clip rectangle as one
            // batch: shapes first, then text on top.
            const auto clipIndex = commands[begin].clipIndex;
            auto end = begin + 1;
            while ((end < range.commandEnd) && (commands[end].clipIndex == clipIndex)) {
                ++end;
            }

            commandList->setScissorRect(toPhysicalRect(clipRects[clipIndex], pixelRatio));

            for (auto i = begin; i < end; ++i) {
                if (commands[i].kind != DrawCommandKind::Text) {
                    drawShape(drawList, commands[i], viewportSize.y, antialiasingWidth / pixelRatio, pixelRatio);
                }
            }
            primitiveBatch_->flush(commandList, primitivePipeline_);

            for (auto i = begin; i < end; ++i) {
                if (commands[i].kind == DrawCommandKind::Text) {
                    drawText(drawList, commands[i], viewportSize.y, pixelRatio);
                }
            }
            spriteBatch_->flush(commandList, fontPipeline_);

            begin = end;
        }
    }

    commandList->setScissorRect(toPhysicalRect(F32Rect2D{0.0f, 0.0f, viewportSize.x, viewportSize.y}, pixelRatio));

    primitiveBatch_->submit(graphicsDevice_);
    spriteBatch_->submit(graphicsDevice_);
}

void RendererImpl::drawShape(const DrawList& drawList, const DrawCommand& command, f32 viewportHeight, f32 fringeWidth, f32 pixelRatio)
{
    const auto flip = [viewportHeight](const Vector2& p) { return Vector2{p.x, viewportHeight - p.y}; };

    // NOTE: Align the edges of rectangles to physical pixels in the top-left
    // space, so that borders keep one width at fractional pixel ratios and do
    // not flicker as the window resizes. The y-up space then stays aligned
    // when the viewport height is a whole number of physical pixels.
    const auto spanX = detail::snapSpanToPixels(command.p0.x, command.p1.x, pixelRatio);
    const auto spanY = detail::snapSpanToPixels(command.p0.y, command.p1.y, pixelRatio);
    const auto left = spanX.start;
    const auto bottom = viewportHeight - (spanY.start + spanY.length);
    const auto width = spanX.length;
    const auto height = spanY.length;
    const auto triangle = [this](const Vector2& a, const Vector2& b, const Vector2& c, const Color& ca, const Color& cb, const Color& cc) {
        primitiveBatch_->drawTriangle(a, b, c, ca, cb, cc);
    };

    switch (command.kind) {
    case DrawCommandKind::FillRect: {
        if ((fringeWidth > 0.0f) && (command.radius >= 0.5f)) {
            fillAntialiasedRoundedRect(*primitiveBatch_, left, bottom, width, height, command.radius, command.color, fringeWidth, pixelRatio);
            break;
        }
        fillRoundedRect(*primitiveBatch_, left, bottom, width, height, command.radius, command.color);
        break;
    }
    case DrawCommandKind::StrokeRect: {
        if ((fringeWidth > 0.0f) && (command.radius >= 0.5f)) {
            strokeAntialiasedRoundedRect(*primitiveBatch_, left, bottom, width, height, command.radius, command.thickness, command.color, fringeWidth, pixelRatio);
            break;
        }
        strokeRoundedRect(*primitiveBatch_, left, bottom, width, height, command.radius, detail::snapThicknessToPixels(command.thickness, pixelRatio), command.color);
        break;
    }
    case DrawCommandKind::FillRectGradient: {
        // NOTE: PrimitiveBatch takes the corner colors counterclockwise from the
        // bottom-left corner of its y-up space.
        const auto& gradient = drawList.getGradient(command);
        primitiveBatch_->drawRectangle(
            Matrix3x2::createScale(1.0f),
            Vector2{left, bottom},
            width,
            height,
            gradient.bottomLeft,
            gradient.bottomRight,
            gradient.topRight,
            gradient.topLeft);
        break;
    }
    case DrawCommandKind::FillCircle: {
        if (fringeWidth > 0.0f) {
            const auto segments = detail::quarterCircleSegments(command.radius, pixelRatio) * 4;
            const auto center = flip(command.p0);
            Contour contour;
            for (i32 i = 0; i < segments; ++i) {
                contour.append(pointOnCircle(center, command.radius, math::TwoPi<f32> * static_cast<f32>(i) / static_cast<f32>(segments)));
            }
            detail::drawAntialiasedConvex(contour.getPoints(), command.color, fringeWidth, triangle);
            break;
        }
        const auto segments = std::clamp(static_cast<i32>(std::ceil(command.radius * 1.5f)) + 8, 12, 64);
        primitiveBatch_->drawCircle(flip(command.p0), command.radius, segments, command.color);
        break;
    }
    case DrawCommandKind::FillTriangle: {
        const std::array<Vector2, 3> points{flip(command.p0), flip(command.p1), flip(command.p2)};
        detail::drawAntialiasedConvex(std::span<const Vector2>{points}, command.color, fringeWidth, triangle);
        break;
    }
    case DrawCommandKind::Line: {
        if ((command.p0.x == command.p1.x) || (command.p0.y == command.p1.y)) {
            // NOTE: Draw a horizontal or vertical line as a rectangle covering
            // whole physical pixels, which is crisp without a fringe.
            const auto thickness = detail::snapThicknessToPixels(command.thickness, pixelRatio);
            const bool horizontal = (command.p0.y == command.p1.y);
            const auto along = horizontal
                                   ? detail::snapSpanToPixels(std::min(command.p0.x, command.p1.x), std::abs(command.p1.x - command.p0.x), pixelRatio)
                                   : detail::snapSpanToPixels(std::min(command.p0.y, command.p1.y), std::abs(command.p1.y - command.p0.y), pixelRatio);
            const auto across = detail::snapToPixels((horizontal ? command.p0.y : command.p0.x) - thickness * 0.5f, pixelRatio);
            const auto pivot = Vector2{0.0f, 0.0f};
            if (horizontal) {
                primitiveBatch_->drawRectangle(Vector2{along.start, viewportHeight - (across + thickness)}, along.length, thickness, pivot, command.color);
            }
            else {
                primitiveBatch_->drawRectangle(Vector2{across, viewportHeight - (along.start + along.length)}, thickness, along.length, pivot, command.color);
            }
            break;
        }
        const std::array<Vector2, 2> points{flip(command.p0), flip(command.p1)};
        detail::drawAntialiasedPolyline(std::span<const Vector2>{points}, command.color, command.thickness, fringeWidth, triangle);
        break;
    }
    case DrawCommandKind::Polyline: {
        const std::array<Vector2, 3> points{flip(command.p0), flip(command.p1), flip(command.p2)};
        detail::drawAntialiasedPolyline(std::span<const Vector2>{points}, command.color, command.thickness, fringeWidth, triangle);
        break;
    }
    case DrawCommandKind::Text: {
        break;
    }
    }
}

void RendererImpl::drawText(const DrawList& drawList, const DrawCommand& command, f32 viewportHeight, f32 pixelRatio)
{
    const auto scale = command.fontSize / fontRasterSize_;

    // TODO: Cache the SDF parameters per font size if profiling shows a cost.
    const auto sdf = computeSpriteFontSDFParameters(SpriteFontSDFDesc{
        .fontSize = fontRasterSize_,
        .effectiveScale = scale * pixelRatio,
        .weight = 0.0f,
        .outlineThickness = 0.0f,
    });

    // NOTE: With the top-left pivot, SpriteFont places the baseline one line
    // spacing below the draw position. Put the baseline and the start of the
    // text on physical pixel edges, so that glyphs do not shimmer as the
    // window resizes.
    const auto baselineY = viewportHeight - detail::snapToPixels(command.p0.y + command.fontSize * baselineRatio, pixelRatio);
    const auto position = Vector2{detail::snapToPixels(command.p0.x, pixelRatio), baselineY + font_->getLineSpacing() * scale};

    SpriteFontDrawParameters params;
    params.color = command.color;
    params.fontSmoothing = sdf.fontSmoothing;
    params.fontWeight = sdf.fontWeight;
    params.originPivot = Vector2{0.0f, 1.0f};
    params.scale = Vector2{scale, scale};
    font_->draw(graphicsDevice_, *spriteBatch_, drawList.getText(command), position, params);
}

} // namespace

Renderer::~Renderer() = default;

std::tuple<std::shared_ptr<Renderer>, std::unique_ptr<Error>>
createRenderer(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<SpriteFont>& font,
    f32 fontRasterSize) noexcept
{
    auto renderer = std::make_shared<RendererImpl>();
    if (auto err = renderer->initialize(fs, graphicsDevice, font, fontRasterSize); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(renderer), nullptr);
}

} // namespace pomdog::devgui
