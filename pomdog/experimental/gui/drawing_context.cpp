// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/experimental/graphics/truetype_font.h"
#include "pomdog/experimental/image/image.h"
#include "pomdog/experimental/image/svg_loader.h"
#include "pomdog/experimental/texture_packer/texture_atlas_generator.h"
#include "pomdog/gpu/blend_descriptor.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/depth_stencil_descriptor.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/rasterizer_descriptor.h"
#include "pomdog/gpu/sampler_descriptor.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/path_helper.h"
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

std::uint32_t MakeFontID(FontWeight fontWeight, FontSize fontSize)
{
    auto fontID = static_cast<std::uint32_t>(fontWeight);
    fontID |= (static_cast<std::uint32_t>(fontSize) << 8);
    return fontID;
}

} // namespace

std::unique_ptr<Error>
DrawingContext::initialize(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<vfs::FileSystemContext>& fs)
{
    viewportWidth = 1;
    viewportHeight = 1;

    primitiveBatch = std::make_shared<PrimitiveBatch>();
    if (auto err = primitiveBatch->initialize(
            fs,
            graphicsDevice,
            std::nullopt,
            gpu::RasterizerDescriptor::createCullNone());
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize PrimitiveBatch");
    }

    spriteBatch = std::make_shared<SpriteBatch>();
    if (auto err = spriteBatch->initialize(
            fs,
            graphicsDevice,
            gpu::BlendDescriptor::createNonPremultiplied(),
            gpu::RasterizerDescriptor::createCullNone(),
            gpu::SamplerDescriptor::createLinearWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Default);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize SpriteBatch");
    }

    std::shared_ptr<TrueTypeFont> fontRegular;
    std::shared_ptr<TrueTypeFont> fontBold;

    if (auto [font, err] = loadTrueTypeFont(fs, "/assets/fonts/Roboto-Medium.ttf"); err != nullptr) {
        Log::Critical("pomdog", "failed to load font file, " + err->toString());
    }
    else {
        fontRegular = std::move(font);
    }

    if (auto [font, err] = loadTrueTypeFont(fs, "/assets/fonts/Roboto-Black.ttf"); err != nullptr) {
        Log::Critical("pomdog", "failed to load font file, " + err->toString());
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

        std::int16_t fontPointSize = 26;
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

        auto fontID = MakeFontID(fontWeight, fontSize);
        auto spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, fontPointSize, fontPointSize);
        spriteFont->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-+*/?&!");
        spriteFont->setDefaultCharacter(U'?');
        spriteFonts.emplace(std::move(fontID), std::move(spriteFont));
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

    std::vector<TexturePacker::TextureAtlasGeneratorSource> sources;
    sources.reserve(svgFiles.size());

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
            image->SetData(std::span<const Color>{
                reinterpret_cast<const Color*>(res.pixelData.data()),
                static_cast<std::size_t>(res.width * res.height),
            });
            POMDOG_CLANG_UNSAFE_BUFFER_END

            for (int y = 0; y < image->GetHeight(); y++) {
                for (int x = 0; x < image->GetWidth(); x++) {
                    auto pixel = image->GetPixel(x, y);
                    pixel.r = 255;
                    pixel.g = 255;
                    pixel.b = 255;
                    image->SetPixel(x, y, pixel);
                }
            }

            TexturePacker::TextureAtlasGeneratorSource src;
            src.Image = std::move(image);
            src.Name = filename;
            sources.push_back(std::move(src));
        }
    }

    auto result = TexturePacker::TextureAtlasGenerator::Generate(sources, 256, 256);
    POMDOG_ASSERT(result.Image);
    POMDOG_ASSERT(!result.HasError);

    // NOTE: Creating texture from packed image
    iconTexture = std::get<0>(graphicsDevice->createTexture2D(
        result.Image->GetWidth(),
        result.Image->GetHeight(),
        false,
        gpu::PixelFormat::R8G8B8A8_UNorm));
    iconTexture->setData(result.Image->GetData());

    iconTextureAtlas = std::move(result.Atlas);

    return nullptr;
}

Point2D DrawingContext::GetCurrentTransform() const
{
    POMDOG_ASSERT(!matrixStack.empty());
    return matrixStack.back();
}

void DrawingContext::PushTransform(const Point2D& position)
{
    matrixStack.push_back(position);
}

void DrawingContext::PopTransform()
{
    POMDOG_ASSERT(!matrixStack.empty());
    matrixStack.pop_back();
}

void DrawingContext::Reset(int viewportWidthIn, int viewportHeightIn)
{
    viewportWidth = viewportWidthIn;
    viewportHeight = viewportHeightIn;
}

std::shared_ptr<SpriteFont>
DrawingContext::GetFont(FontWeight fontWeight, FontSize fontSize) const
{
    auto fontID = MakeFontID(fontWeight, fontSize);
    auto iter = spriteFonts.find(fontID);

    POMDOG_ASSERT(iter != std::end(spriteFonts));
    if (iter != std::end(spriteFonts)) {
        return iter->second;
    }
    return nullptr;
}

void DrawingContext::PushScissorRect(const Rect2D& scissorRect)
{
    auto parentRect = Rect2D{0, 0, viewportWidth, viewportHeight};
    if (!scissorRects.empty()) {
        parentRect = scissorRects.back();
    }

    auto rect = scissorRect;

    // NOTE: Flip Y-coordinate
    rect.y = viewportHeight - scissorRect.y - scissorRect.height;

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
    else if (rect.x > viewportWidth) {
        rect.x = viewportWidth;
        rect.width = 0;
    }
    else if (rect.x + rect.width > viewportWidth) {
        const auto diff = (rect.x + rect.width) - viewportWidth;
        rect.width = rect.width - diff;
    }

    if (rect.y < 0) {
        rect.height = std::max(rect.height + rect.y, 0);
        rect.y = 0;
    }
    else if (rect.y > viewportHeight) {
        rect.y = viewportHeight;
        rect.height = 0;
    }
    else if (rect.y + rect.height > viewportHeight) {
        const auto diff = (rect.y + rect.height) - viewportHeight;
        rect.height = rect.height - diff;
    }

    primitiveBatch->flush();
    spriteBatch->flush();

    commandList->setScissorRect(rect);
    scissorRects.push_back(rect);
}

void DrawingContext::PopScissorRect()
{
    POMDOG_ASSERT(!scissorRects.empty());
    scissorRects.pop_back();

    auto scissorRect = Rect2D{0, 0, viewportWidth, viewportHeight};
    if (!scissorRects.empty()) {
        scissorRect = scissorRects.back();
    }
    commandList->setScissorRect(scissorRect);
}

void DrawingContext::BeginDraw(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    commandList = commandListIn;

    primitiveBatch->begin(commandList, transformMatrix);
    spriteBatch->begin(commandList, transformMatrix);
}

void DrawingContext::EndDraw()
{
    primitiveBatch->end();
    spriteBatch->end();
}

void DrawingContext::DrawIcon(
    const std::string& name,
    const Vector2& position,
    const Color& color,
    const Radian<f32>& rotation,
    const Vector2& originPivot,
    float scale)
{
    if (iconTexture == nullptr) {
        return;
    }
    if (iconTextureAtlas.regions.empty()) {
        return;
    }

    auto* frame = &iconTextureAtlas.regions.front();

    auto iter = std::find_if(
        std::begin(iconTextureAtlas.regions),
        std::end(iconTextureAtlas.regions),
        [&](const auto& r) -> bool { return r.Name == name; });
    if (iter != std::end(iconTextureAtlas.regions)) {
        frame = &(*iter);
    }

    POMDOG_ASSERT(frame != nullptr);

    spriteBatch->draw(
        iconTexture,
        position,
        frame->Region,
        color,
        rotation,
        originPivot,
        scale);
}

PrimitiveBatch* DrawingContext::GetPrimitiveBatch()
{
    return primitiveBatch.get();
}

SpriteBatch* DrawingContext::GetSpriteBatch()
{
    return spriteBatch.get();
}

const ColorScheme* DrawingContext::GetColorScheme() const
{
    return &colorScheme;
}

} // namespace pomdog::gui
