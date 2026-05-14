// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/basic/types.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/image/image.h"
#include "pomdog/experimental/image/svg_loader.h"
#include "pomdog/experimental/texture_atlas/texture_atlas_dynamic.h"
#include "pomdog/gpu/blend_desc.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/depth_stencil_desc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/rasterizer_desc.h"
#include "pomdog/gpu/sampler_desc.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gui {
namespace {

u32 MakeFontID(FontWeight fontWeight, FontSize fontSize)
{
    auto fontID = static_cast<u32>(fontWeight);
    fontID |= (static_cast<u32>(fontSize) << 8);
    return fontID;
}

} // namespace

DrawingContext::DrawingContext() = default;

std::unique_ptr<Error>
DrawingContext::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<vfs::FileSystemContext>& fs)
{
    graphicsDevice_ = graphicsDevice;
    viewportWidth_ = 1;
    viewportHeight_ = 1;

    if (auto [p, err] = createPrimitivePipeline(
            fs,
            graphicsDevice,
            std::nullopt,
            gpu::RasterizerDesc::createCullNone());
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice); err != nullptr) {
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
            SpriteBatchPixelShaderMode::Sprite);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
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
        return errors::wrap(std::move(err), "failed to create SpritePipeline (DistanceField)");
    }
    else {
        spritePipelineFont_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice, 1024); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice, 2048); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch (font)");
    }
    else {
        spriteBatchFont_ = std::move(p);
    }

    std::shared_ptr<TrueTypeFont> fontRegular;
    std::shared_ptr<TrueTypeFont> fontBold;

    if (auto [font, err] = loadTrueTypeFont(fs, "/assets/fonts/Roboto-Medium.ttf"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load font file");
    }
    else {
        fontRegular = std::move(font);
    }

    if (auto [font, err] = loadTrueTypeFont(fs, "/assets/fonts/Roboto-Black.ttf"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load font file");
    }
    else {
        fontBold = std::move(font);
    }

    std::array<std::pair<FontWeight, FontSize>, 9> fontPairs = {{
        {FontWeight::Light, FontSize::Small},
        {FontWeight::Light, FontSize::Medium},
        {FontWeight::Light, FontSize::Large},
        {FontWeight::Normal, FontSize::Small},
        {FontWeight::Normal, FontSize::Medium},
        {FontWeight::Normal, FontSize::Large},
        {FontWeight::Bold, FontSize::Small},
        {FontWeight::Bold, FontSize::Medium},
        {FontWeight::Bold, FontSize::Large},
    }};

    for (auto& pair : fontPairs) {
        const auto fontWeight = pair.first;
        const auto fontSize = pair.second;

        auto font = fontRegular;
        switch (fontWeight) {
        case FontWeight::Light:
        case FontWeight::Normal:
            font = fontRegular;
            break;
        case FontWeight::Bold:
            font = fontBold;
            break;
        }

        i16 fontPointSize = 26;
        switch (fontSize) {
        case FontSize::Small:
            fontPointSize = 13;
            break;
        case FontSize::Medium:
            fontPointSize = 14;
            break;
        case FontSize::Large:
            fontPointSize = 26;
            break;
        }

        constexpr bool useSDF = true;

        auto fontID = MakeFontID(fontWeight, fontSize);
        if (auto [p, err] = createSpriteFont(
                graphicsDevice,
                font,
                fontPointSize,
                fontPointSize,
                useSDF);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create SpriteFont");
        }
        else {
            p->setDefaultCharacter(U'?');
            spriteFonts_.emplace(std::move(fontID), std::move(p));
        }
    }

    const auto svgFiles = {
        "ios-arrow-back.svg",
        "ios-arrow-down.svg",
        "ios-arrow-forward.svg",
        "ios-arrow-up.svg",
        "ios-search.svg",
        "ios-square-outline.svg",
        "ios-square.svg",
        "ios-volume-high.svg",
        "ios-volume-low.svg",
        "ios-volume-mute.svg",
        "ios-volume-off.svg",
        "md-add.svg",
        "md-arrow-back.svg",
        "md-arrow-down.svg",
        "md-arrow-dropdown.svg",
        "md-arrow-dropleft.svg",
        "md-arrow-dropright.svg",
        "md-arrow-dropup.svg",
        "md-arrow-forward.svg",
        "md-arrow-up.svg",
        "md-backspace.svg",
        "md-build.svg",
        "md-camera.svg",
        "md-checkmark.svg",
        "md-close-circle-outline.svg",
        "md-close-circle.svg",
        "md-close.svg",
        "md-construct.svg",
        "md-copy.svg",
        "md-document.svg",
        "md-download.svg",
        "md-expand.svg",
        "md-eye-off.svg",
        "md-eye.svg",
        "md-lock.svg",
        "md-refresh.svg",
        "md-star-outline.svg",
        "md-star.svg",
    };

    auto iconAtlas = createTextureAtlasDynamic();

    const auto svgDir = std::string("/assets/svg");

    constexpr int canvasWidth = 32;
    constexpr int canvasHeight = 32;

    // Generating texture atlas from SVG image
    for (const auto& filename : svgFiles) {
        // NOTE: Load SVG texture.
        auto filePath = filepaths::joinUnix(svgDir, filename);

        auto [file, openErr] = vfs::open(fs, filePath);
        if (openErr != nullptr) {
            return errors::wrap(std::move(openErr), "failed to open svg file: " + filePath);
        }

        auto [info, statErr] = file->stat();
        if (statErr != nullptr) {
            return errors::wrap(std::move(statErr), "failed to stat svg file: " + filePath);
        }

        std::vector<u8> svgData(static_cast<std::size_t>(info.size));
        auto [bytesRead, readErr] = file->read(std::span<u8>(svgData));
        if (readErr != nullptr) {
            return errors::wrap(std::move(readErr), "failed to read svg file: " + filePath);
        }

        if (auto [res, err] = decodeSVG(std::span<const u8>(svgData), canvasWidth, canvasHeight); err != nullptr) {
            return errors::wrap(std::move(err), "failed to load texture: " + filePath);
        }
        else if (res.pixelData.empty()) {
            return errors::make("empty pixel data: " + filePath);
        }
        else if (res.width <= 0 || res.height <= 0) {
            return errors::make("invalid image size: " + filePath);
        }
        else if (res.width * res.height > static_cast<i32>(res.pixelData.size() / sizeof(Color))) {
            return errors::make("mismatched image size: " + filePath);
        }
        else {
            auto image = std::make_shared<Image>(res.width, res.height);

            POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
            image->setData(std::span<const Color>{
                reinterpret_cast<const Color*>(res.pixelData.data()),
                static_cast<std::size_t>(res.width * res.height),
            });
            POMDOG_CLANG_UNSAFE_BUFFER_END

            for (int y = 0; y < image->getHeight(); y++) {
                for (int x = 0; x < image->getWidth(); x++) {
                    auto pixel = image->getPixel(x, y);
                    pixel.r = 255;
                    pixel.g = 255;
                    pixel.b = 255;
                    image->setPixel(x, y, pixel);
                }
            }

            iconAtlas->add(filename, image, false);
        }
    }

    if (auto err = iconAtlas->build(graphicsDevice, 256, 256); err != nullptr) {
        return errors::wrap(std::move(err), "failed to build icon atlas");
    }
    iconAtlas_ = iconAtlas;
    iconTexture_ = iconAtlas_->getTexture();

    return nullptr;
}

Point2D DrawingContext::getCurrentTransform() const
{
    POMDOG_ASSERT(!matrixStack_.empty());
    return matrixStack_.back();
}

void DrawingContext::pushTransform(const Point2D& position)
{
    matrixStack_.push_back(position);
}

void DrawingContext::popTransform()
{
    POMDOG_ASSERT(!matrixStack_.empty());
    matrixStack_.pop_back();
}

void DrawingContext::reset(int viewportWidthIn, int viewportHeightIn)
{
    viewportWidth_ = viewportWidthIn;
    viewportHeight_ = viewportHeightIn;
}

std::shared_ptr<SpriteFont>
DrawingContext::getFont(FontWeight fontWeight, FontSize fontSize) const
{
    auto fontID = MakeFontID(fontWeight, fontSize);
    auto iter = spriteFonts_.find(fontID);

    POMDOG_ASSERT(iter != std::end(spriteFonts_));
    if (iter != std::end(spriteFonts_)) {
        return iter->second;
    }
    return nullptr;
}

void DrawingContext::pushScissorRect(const Rect2D& scissorRect)
{
    auto parentRect = Rect2D{0, 0, viewportWidth_, viewportHeight_};
    if (!scissorRects_.empty()) {
        parentRect = scissorRects_.back();
    }

    auto rect = scissorRect;

    // NOTE: Flip Y-coordinate
    rect.y = viewportHeight_ - scissorRect.y - scissorRect.height;

    if ((parentRect.width <= 0) && (parentRect.height <= 0)) {
        rect.width = 0;
        rect.height = 0;
    }
    else if (parentRect.contains(rect)) {
        // NOTE: Not to do
    }
    else if (parentRect.intersects(rect)) {
        if (rect.x < parentRect.x) {
            const auto offset = parentRect.x - rect.x;
            rect.x = parentRect.x;
            rect.width = std::min(rect.width - offset, parentRect.width);
        }
        else {
            rect.width = std::min(parentRect.width - (rect.x - parentRect.x), rect.width);
        }

        if (rect.y < parentRect.y) {
            const auto offset = parentRect.y - rect.y;
            rect.y = parentRect.y;
            rect.height = std::max(rect.height - offset, 0);
        }
        else {
            rect.height = std::min(parentRect.height - (rect.y - parentRect.y), rect.height);
        }
    }
    else {
        rect.width = 0;
        rect.height = 0;
    }

    if (rect.x < 0) {
        rect.width = std::max(rect.width + rect.x, 0);
        rect.x = 0;
    }
    else if (rect.x > viewportWidth_) {
        rect.x = viewportWidth_;
        rect.width = 0;
    }
    else if (rect.x + rect.width > viewportWidth_) {
        const auto diff = (rect.x + rect.width) - viewportWidth_;
        rect.width = rect.width - diff;
    }

    if (rect.y < 0) {
        rect.height = std::max(rect.height + rect.y, 0);
        rect.y = 0;
    }
    else if (rect.y > viewportHeight_) {
        rect.y = viewportHeight_;
        rect.height = 0;
    }
    else if (rect.y + rect.height > viewportHeight_) {
        const auto diff = (rect.y + rect.height) - viewportHeight_;
        rect.height = rect.height - diff;
    }

    flushPrimitiveBatch();
    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatchFont_->flush(commandList_, spritePipelineFont_);

    commandList_->setScissorRect(rect);
    scissorRects_.push_back(rect);
}

void DrawingContext::popScissorRect()
{
    POMDOG_ASSERT(!scissorRects_.empty());
    scissorRects_.pop_back();

    auto scissorRect = Rect2D{0, 0, viewportWidth_, viewportHeight_};
    if (!scissorRects_.empty()) {
        scissorRect = scissorRects_.back();
    }
    commandList_->setScissorRect(scissorRect);
}

void DrawingContext::beginDraw(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    commandList_ = commandListIn;

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(transformMatrix);
    spriteBatch_->reset();
    spriteBatch_->setTransform(transformMatrix);
    spriteBatchFont_->reset();
    spriteBatchFont_->setTransform(
        transformMatrix,
        0.105f,
        0.480f,
        Color{34, 31, 29, 255},
        0.102f);
}

void DrawingContext::endDraw()
{
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);
    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);
    spriteBatchFont_->flush(commandList_, spritePipelineFont_);
    spriteBatchFont_->submit(graphicsDevice_);
}

void DrawingContext::drawIcon(
    const std::string& name,
    const Vector2& position,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    float scale)
{
    if (iconAtlas_ == nullptr || iconTexture_ == nullptr) {
        return;
    }

    const auto key = computeStringHash32(name);

    TextureRegion region = {};
    if (auto idx = iconAtlas_->findRegionByKey(key); idx) {
        region = iconAtlas_->getRegion(*idx);
    }
    else {
        region = iconAtlas_->getRegion(0);
    }

    spriteBatch_->draw(
        iconTexture_,
        position,
        region,
        color,
        rotation,
        originPivot,
        scale);
}

PrimitiveBatch* DrawingContext::getPrimitiveBatch()
{
    return primitiveBatch_.get();
}

SpriteBatch* DrawingContext::getSpriteBatch()
{
    return spriteBatchFont_.get();
}

void DrawingContext::flushPrimitiveBatch()
{
    primitiveBatch_->flush(commandList_, primitivePipeline_);
}

void DrawingContext::flushSpriteBatch()
{
    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatchFont_->flush(commandList_, spritePipelineFont_);
}

const ColorScheme* DrawingContext::getColorScheme() const
{
    return &colorScheme_;
}

const std::shared_ptr<gpu::GraphicsDevice>& DrawingContext::getGraphicsDevice() const
{
    return graphicsDevice_;
}

} // namespace pomdog::gui
