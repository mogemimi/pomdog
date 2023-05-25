// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/drawing_context.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/experimental/graphics/truetype_font_loader.h"
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

namespace pomdog::gui {
namespace {

std::uint32_t MakeFontID(FontWeight fontWeight, FontSize fontSize)
{
    auto fontID = static_cast<std::uint32_t>(fontWeight);
    fontID |= (static_cast<std::uint32_t>(fontSize) << 8);
    return fontID;
}

} // namespace

DrawingContext::DrawingContext(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : viewportWidth(1)
    , viewportHeight(1)
{
    primitiveBatch = std::make_shared<PrimitiveBatch>(
        graphicsDevice,
        std::nullopt,
        gpu::RasterizerDescriptor::CreateCullNone(),
        assets);

    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        gpu::BlendDescriptor::CreateNonPremultiplied(),
        gpu::RasterizerDescriptor::CreateCullNone(),
        gpu::SamplerDescriptor::CreateLinearWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        assets);

    std::shared_ptr<TrueTypeFont> fontRegular;
    std::shared_ptr<TrueTypeFont> fontBold;
    std::unique_ptr<Error> fontErr;

    std::tie(fontRegular, fontErr) = assets.Load<TrueTypeFont>("Fonts/Roboto/Roboto-Medium.ttf");
    if (fontErr != nullptr) {
        Log::Critical("pomdog", "failed to load font file, " + fontErr->ToString());
    }

    std::tie(fontBold, fontErr) = assets.Load<TrueTypeFont>("Fonts/Roboto/Roboto-Black.ttf");
    if (fontErr != nullptr) {
        Log::Critical("pomdog", "failed to load font file, " + fontErr->ToString());
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
        spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-+*/?&!");
        spriteFont->SetDefaultCharacter(U'?');
        spriteFonts.emplace(std::move(fontID), std::move(spriteFont));
    }

    const auto svgFiles = {
        "ionicons/ios-arrow-back.svg",
        "ionicons/ios-arrow-down.svg",
        "ionicons/ios-arrow-forward.svg",
        "ionicons/ios-arrow-up.svg",
        "ionicons/ios-search.svg",
        "ionicons/ios-square-outline.svg",
        "ionicons/ios-square.svg",
        "ionicons/ios-volume-high.svg",
        "ionicons/ios-volume-low.svg",
        "ionicons/ios-volume-mute.svg",
        "ionicons/ios-volume-off.svg",
        "ionicons/md-add.svg",
        "ionicons/md-arrow-back.svg",
        "ionicons/md-arrow-down.svg",
        "ionicons/md-arrow-dropdown.svg",
        "ionicons/md-arrow-dropleft.svg",
        "ionicons/md-arrow-dropright.svg",
        "ionicons/md-arrow-dropup.svg",
        "ionicons/md-arrow-forward.svg",
        "ionicons/md-arrow-up.svg",
        "ionicons/md-backspace.svg",
        "ionicons/md-build.svg",
        "ionicons/md-camera.svg",
        "ionicons/md-checkmark.svg",
        "ionicons/md-close-circle-outline.svg",
        "ionicons/md-close-circle.svg",
        "ionicons/md-close.svg",
        "ionicons/md-construct.svg",
        "ionicons/md-copy.svg",
        "ionicons/md-document.svg",
        "ionicons/md-download.svg",
        "ionicons/md-expand.svg",
        "ionicons/md-eye-off.svg",
        "ionicons/md-eye.svg",
        "ionicons/md-lock.svg",
        "ionicons/md-refresh.svg",
        "ionicons/md-star-outline.svg",
        "ionicons/md-star.svg",
    };

    std::vector<TexturePacker::TextureAtlasGeneratorSource> sources;
    sources.reserve(svgFiles.size());

    const auto svgDir = PathHelper::Normalize(PathHelper::Join(assets.GetContentDirectory(), "SVG"));

    constexpr int canvasWidth = 32;
    constexpr int canvasHeight = 32;

    // Generating texture atlas from SVG image
    for (const auto& filename : svgFiles) {
        // NOTE: Load SVG texture.
        auto filePath = PathHelper::Join(svgDir, filename);

        if (auto [res, err] = SVG::DecodeFile(filePath, canvasWidth, canvasHeight); err != nullptr) {
            Log::Verbose("failed to load texture: " + err->ToString());
        }
        else {
            auto image = std::make_shared<Image>(res.Width, res.Height);
            image->SetData(reinterpret_cast<const Color*>(res.PixelData));

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
    iconTexture = std::get<0>(graphicsDevice->CreateTexture2D(
        result.Image->GetWidth(),
        result.Image->GetHeight(),
        false,
        PixelFormat::R8G8B8A8_UNorm));
    iconTexture->SetData(result.Image->GetData());

    iconTextureAtlas = std::move(result.Atlas);
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

void DrawingContext::PushScissorRect(const Rectangle& scissorRect)
{
    auto parentRect = Rectangle{0, 0, viewportWidth, viewportHeight};
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

    primitiveBatch->Flush();
    spriteBatch->Flush();

    commandList->SetScissorRect(rect);
    scissorRects.push_back(rect);
}

void DrawingContext::PopScissorRect()
{
    POMDOG_ASSERT(!scissorRects.empty());
    scissorRects.pop_back();

    auto scissorRect = Rectangle{0, 0, viewportWidth, viewportHeight};
    if (!scissorRects.empty()) {
        scissorRect = scissorRects.back();
    }
    commandList->SetScissorRect(scissorRect);
}

void DrawingContext::BeginDraw(
    const std::shared_ptr<gpu::CommandList>& commandListIn,
    const Matrix4x4& transformMatrix)
{
    commandList = commandListIn;

    primitiveBatch->Begin(commandList, transformMatrix);
    spriteBatch->Begin(commandList, transformMatrix);
}

void DrawingContext::EndDraw()
{
    primitiveBatch->End();
    spriteBatch->End();
}

void DrawingContext::DrawIcon(
    const std::string& name,
    const Vector2& position,
    const Color& color,
    const Radian<float>& rotation,
    const Vector2& originPivot,
    float scale)
{
    auto* frame = &iconTextureAtlas.regions.front();

    auto iter = std::find_if(
        std::begin(iconTextureAtlas.regions),
        std::end(iconTextureAtlas.regions),
        [&](const auto& r) -> bool { return r.Name == name; });
    if (iter != std::end(iconTextureAtlas.regions)) {
        frame = &(*iter);
    }

    POMDOG_ASSERT(frame != nullptr);

    spriteBatch->Draw(
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
