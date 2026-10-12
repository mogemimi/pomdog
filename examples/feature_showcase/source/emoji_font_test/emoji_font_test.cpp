#include "emoji_font_test.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/experimental/i18n/text_wrap.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

constexpr f32 fontSize = 32.0f;
constexpr f32 emojiScale = 1.0f;
constexpr f32 textScale = 0.5f;
constexpr f32 titleScale = 0.42f;
constexpr f32 margin = 20.0f;
constexpr f32 headerHeight = 48.0f;
constexpr f32 rowHeight = 46.0f;
constexpr f32 paragraphWidthRatio = 0.6f;

struct EmojiRow final {
    u8 red;
    u8 green;
    u8 blue;
    std::string_view text;
};

// NOTE: Noto Emoji is a monochrome outline font, so each row is tinted.
// Sequences that need a variation selector or a joiner are left out because
// SpriteFont draws one glyph per code point.
constexpr std::array<EmojiRow, 4> emojiRows = {{
    {255, 214, 102, "😀 😃 😄 😁 😆 😅 😂 🙂 😉 😊 😇 😎"},
    {240, 170, 120, "🐶 🐱 🐭 🐹 🐰 🦊 🐻 🐼 🐨 🐯 🦁 🐸"},
    {250, 120, 120, "🍎 🍊 🍋 🍉 🍇 🍓 🍒 🍑 🍍 🍣 🍙 🍰"},
    {130, 200, 250, "⚽ 🏀 🎮 🎲 🎯 🎸 🏆 🎁 💎 🔑 🔥 ⭐"},
}};

constexpr std::string_view paragraph =
    "Pomdog 🐶 draws emoji 🎉 from a fallback font, so text and pictographs "
    "🍣🍜🍰🍩🍪 wrap together with i18n::wrapText ✨";

constexpr std::string_view title = "Noto Sans with Noto Emoji as a fallback font";

} // namespace

EmojiFontTest::EmojiFontTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
EmojiFontTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }
    if (auto [p, err] = createPrimitivePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceField);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    auto [textFont, textFontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (textFontErr != nullptr) {
        return errors::wrap(std::move(textFontErr), "failed to load a font file");
    }
    // NOTE: The engine assets provide Noto Emoji to every application.
    auto [emojiFont, emojiFontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoEmoji-Medium.ttf");
    if (emojiFontErr != nullptr) {
        return errors::wrap(std::move(emojiFontErr), "failed to load a font file");
    }

    // NOTE: Glyphs missing from Noto Sans, such as emoji, fall back to Noto Emoji.
    const std::vector<std::shared_ptr<TrueTypeFont>> fonts = {textFont, emojiFont};
    const std::vector<f32> fontSizes = {fontSize, fontSize};
    if (auto [p, err] = createSpriteFont(graphicsDevice_, fonts, fontSizes, 40.0f, true); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }

    // NOTE: Rasterize every glyph up front so that wrapping can measure with
    // cached glyphs only, without touching the GPU.
    std::string allText{title};
    allText.append(paragraph);
    for (const auto& row : emojiRows) {
        allText.append(row.text);
    }
    spriteFont_->prepareFonts(graphicsDevice_, allText);

    return nullptr;
}

void EmojiFontTest::update()
{
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();
    const auto wrapWidth = std::max(1.0f, (static_cast<f32>(clientBounds.width) - margin * 2.0f) * paragraphWidthRatio);
    if (std::abs(wrapWidth - wrappedWidth_) < 0.5f) {
        return;
    }

    const auto measure = [this](std::string_view text) -> f32 {
        return spriteFont_->measureString(nullptr, std::string{text}).x * textScale;
    };
    wrappedText_ = i18n::wrapText(measure, paragraph, wrapWidth, i18n::LineBreakLanguage::Default).wrappedText;
    wrappedWidth_ = wrapWidth;
}

void EmojiFontTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();
    const auto pixelRatio = gameHost_->getWindow()->getPixelRatio();
    const auto emojiSDFParameters = computeSpriteFontSDFParameters(
        SpriteFontSDFDesc{.fontSize = fontSize, .effectiveScale = emojiScale * pixelRatio});
    const auto textSDFParameters = computeSpriteFontSDFParameters(
        SpriteFontSDFDesc{.fontSize = fontSize, .effectiveScale = textScale * pixelRatio});
    const auto titleSDFParameters = computeSpriteFontSDFParameters(
        SpriteFontSDFDesc{.fontSize = fontSize, .effectiveScale = titleScale * pixelRatio});

    const auto clientWidth = static_cast<f32>(clientBounds.width);
    const auto clientHeight = static_cast<f32>(clientBounds.height);
    const auto projectionMatrix = Matrix4x4::createOrthographicLH(clientWidth, clientHeight, 0.0f, 100.0f);

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color{36, 38, 46, 255}.toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    const auto left = -clientWidth * 0.5f + margin;
    const auto rowsTop = clientHeight * 0.5f - headerHeight;
    const auto paragraphTop = rowsTop - rowHeight * static_cast<f32>(emojiRows.size()) - margin;
    const auto paragraphBottom = -clientHeight * 0.5f + margin;

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawRectangle(
        Matrix3x2::createIdentity(),
        Vector2{left, paragraphBottom},
        wrappedWidth_,
        std::max(0.0f, paragraphTop - paragraphBottom),
        Color{64, 68, 84, 255});
    primitiveBatch_->drawLine(
        Vector2{left + wrappedWidth_, paragraphBottom},
        Vector2{left + wrappedWidth_, paragraphTop},
        Color{255, 176, 64, 255},
        1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);

    spriteFont_->draw(
        graphicsDevice_,
        *spriteBatch_,
        std::string{title},
        Vector2{clientWidth * 0.5f - margin * 0.5f, clientHeight * 0.5f - 12.0f},
        SpriteFontDrawParameters{
            .color = Color{200, 204, 214, 255},
            .fontSmoothing = titleSDFParameters.fontSmoothing,
            .fontWeight = titleSDFParameters.fontWeight,
            .originPivot = Vector2{1.0f, 1.0f},
            .scale = Vector2{titleScale, titleScale},
        });

    auto rowTop = rowsTop;
    for (const auto& row : emojiRows) {
        spriteFont_->draw(
            graphicsDevice_,
            *spriteBatch_,
            std::string{row.text},
            Vector2{left, rowTop},
            SpriteFontDrawParameters{
                .color = Color{row.red, row.green, row.blue, 255},
                .fontSmoothing = emojiSDFParameters.fontSmoothing,
                .fontWeight = emojiSDFParameters.fontWeight,
                .originPivot = Vector2{0.0f, 1.0f},
                .scale = Vector2{emojiScale, emojiScale},
            });
        rowTop -= rowHeight;
    }

    spriteFont_->draw(
        graphicsDevice_,
        *spriteBatch_,
        wrappedText_,
        Vector2{left, paragraphTop},
        SpriteFontDrawParameters{
            .color = Color::createWhite(),
            .fontSmoothing = textSDFParameters.fontSmoothing,
            .fontWeight = textSDFParameters.fontWeight,
            .originPivot = Vector2{0.0f, 1.0f},
            .scale = Vector2{textScale, textScale},
        });

    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();
    commandList_->close();

    commandQueue_->pushBackCommandList(commandList_);
}

} // namespace feature_showcase
