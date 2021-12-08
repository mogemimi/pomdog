// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/gui/drawing_context.hpp"
#include "pomdog/content/asset_manager.hpp"
#include "pomdog/experimental/graphics/truetype_font_loader.hpp"
#include "pomdog/experimental/image/image.hpp"
#include "pomdog/experimental/image/svg_loader.hpp"
#include "pomdog/experimental/texture_packer/texture_atlas_generator.hpp"
#include "pomdog/graphics/blend_description.hpp"
#include "pomdog/graphics/depth_stencil_description.hpp"
#include "pomdog/graphics/graphics_command_list.hpp"
#include "pomdog/graphics/graphics_device.hpp"
#include "pomdog/graphics/rasterizer_description.hpp"
#include "pomdog/graphics/sampler_description.hpp"
#include "pomdog/graphics/texture2d.hpp"
#include "pomdog/logging/log.hpp"

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
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    AssetManager& assets)
    : viewportWidth(1)
    , viewportHeight(1)
{
    primitiveBatch = std::make_shared<PrimitiveBatch>(
        graphicsDevice,
        std::nullopt,
        RasterizerDescription::CreateCullNone(),
        assets);

    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateNonPremultiplied(),
        RasterizerDescription::CreateCullNone(),
        SamplerDescription::CreateLinearWrap(),
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
                    pixel.R = 255;
                    pixel.G = 255;
                    pixel.B = 255;
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
        SurfaceFormat::R8G8B8A8_UNorm));
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
    rect.Y = viewportHeight - scissorRect.Y - scissorRect.Height;

    if ((parentRect.Width <= 0) && (parentRect.Height <= 0)) {
        rect.Width = 0;
        rect.Height = 0;
    }
    else if (parentRect.Contains(rect)) {
        // NOTE: Not to do
    }
    else if (parentRect.Intersects(rect)) {
        if (rect.X < parentRect.X) {
            const auto offset = parentRect.X - rect.X;
            rect.X = parentRect.X;
            rect.Width = std::min(rect.Width - offset, parentRect.Width);
        }
        else {
            rect.Width = std::min(parentRect.Width - (rect.X - parentRect.X), rect.Width);
        }

        if (rect.Y < parentRect.Y) {
            const auto offset = parentRect.Y - rect.Y;
            rect.Y = parentRect.Y;
            rect.Height = std::max(rect.Height - offset, 0);
        }
        else {
            rect.Height = std::min(parentRect.Height - (rect.Y - parentRect.Y), rect.Height);
        }
    }
    else {
        rect.Width = 0;
        rect.Height = 0;
    }

    if (rect.X < 0) {
        rect.Width = std::max(rect.Width + rect.X, 0);
        rect.X = 0;
    }
    else if (rect.X > viewportWidth) {
        rect.X = viewportWidth;
        rect.Width = 0;
    }
    else if (rect.X + rect.Width > viewportWidth) {
        auto diff = (rect.X + rect.Width) - viewportWidth;
        rect.Width = rect.Width - diff;
    }

    if (rect.Y < 0) {
        rect.Height = std::max(rect.Height + rect.Y, 0);
        rect.Y = 0;
    }
    else if (rect.Y > viewportHeight) {
        rect.Y = viewportHeight;
        rect.Height = 0;
    }
    else if (rect.Y + rect.Height > viewportHeight) {
        auto diff = (rect.Y + rect.Height) - viewportHeight;
        rect.Height = rect.Height - diff;
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
    const std::shared_ptr<GraphicsCommandList>& commandListIn,
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
