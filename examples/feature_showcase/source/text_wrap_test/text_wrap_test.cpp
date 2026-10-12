#include "text_wrap_test.h"
#include "pomdog/experimental/i18n/text_wrap.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

constexpr f32 fontSize = 32.0f;
constexpr f32 textScale = 0.5f;
constexpr f32 labelScale = 0.42f;
constexpr f32 headerHeight = 44.0f;
constexpr f32 cellMargin = 10.0f;
constexpr f32 cellPadding = 8.0f;
constexpr f32 labelHeight = 18.0f;
constexpr f32 minWrapRatio = 0.4f;
constexpr int columnCount = 2;
constexpr int rowCount = 3;

struct SampleDesc final {
    std::string_view label;
    std::string_view fontPath;
    f32 lineSpacing;
    i18n::LineBreakLanguage language;
    std::string_view text;
};

// NOTE: Each sample uses its own font, with Noto Sans as a fallback for
// glyphs such as Latin digits that the font may lack.
constexpr std::array<SampleDesc, columnCount * rowCount> sampleDescs = {{
    {
        "English (Default)",
        "",
        40.0f,
        i18n::LineBreakLanguage::Default,
        "The ancient sword glows. Equip it to deal 1,250 bonus damage to undead enemies!",
    },
    {
        "日本語 (Japanese)",
        "/assets/fonts/NotoSansJP-Regular.ttf",
        40.0f,
        i18n::LineBreakLanguage::Japanese,
        "「古の剣」がかすかに光っている。装備すると、アンデッドに1250の追加ダメージを与える。",
    },
    {
        "简体中文 (Chinese)",
        "/assets/fonts/NotoSansSC-Regular.ttf",
        40.0f,
        i18n::LineBreakLanguage::Chinese,
        "「古剑」散发着微弱的光芒。装备后，对不死系敌人造成1250点额外伤害！",
    },
    {
        "繁體中文 (Chinese)",
        "/assets/fonts/NotoSansTC-Regular.ttf",
        40.0f,
        i18n::LineBreakLanguage::Chinese,
        "「古劍」散發著微弱的光芒。裝備後，對不死系敵人造成1250點額外傷害！",
    },
    {
        "한국어 (Korean)",
        "/assets/fonts/NotoSansKR-Regular.ttf",
        40.0f,
        i18n::LineBreakLanguage::Korean,
        "고대의 검이 희미하게 빛나고 있다. 장착하면 언데드 적에게 1250의 추가 피해를 준다!",
    },
    {
        "ภาษาไทย (Thai)",
        "/assets/fonts/NotoSansThai-Regular.ttf",
        46.0f,
        i18n::LineBreakLanguage::Thai,
        "ดาบโบราณเปล่งแสงจางๆ สวมใส่เพื่อเพิ่มความเสียหาย 1250 หน่วยต่อศัตรูอันเดด",
    },
}};

struct CellLayout final {
    f32 left = 0.0f;
    f32 top = 0.0f;
    f32 width = 0.0f;
    f32 height = 0.0f;
};

[[nodiscard]] CellLayout
computeCellLayout(const Rect2D& clientBounds, int index)
{
    const auto clientWidth = static_cast<f32>(clientBounds.width);
    const auto clientHeight = static_cast<f32>(clientBounds.height);
    const auto column = index % columnCount;
    const auto row = index / columnCount;

    CellLayout cell;
    cell.width = (clientWidth - cellMargin * static_cast<f32>(columnCount + 1)) / static_cast<f32>(columnCount);
    cell.height = (clientHeight - headerHeight - cellMargin * static_cast<f32>(rowCount)) / static_cast<f32>(rowCount);
    cell.left = -clientWidth * 0.5f + cellMargin + static_cast<f32>(column) * (cell.width + cellMargin);
    cell.top = clientHeight * 0.5f - headerHeight - static_cast<f32>(row) * (cell.height + cellMargin);
    return cell;
}

[[nodiscard]] f32
computeWrapWidth(const Rect2D& clientBounds, f32 wrapRatio)
{
    const auto cell = computeCellLayout(clientBounds, 0);
    return std::max(1.0f, (cell.width - cellPadding * 2.0f) * wrapRatio);
}

} // namespace

TextWrapTest::TextWrapTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
TextWrapTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
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

    auto [latinFont, latinFontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (latinFontErr != nullptr) {
        return errors::wrap(std::move(latinFontErr), "failed to load a font file");
    }
    if (auto [p, err] = createSpriteFont(graphicsDevice_, latinFont, fontSize, fontSize, true); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        hintFont_ = std::move(p);
    }

    for (const auto& desc : sampleDescs) {
        std::vector<std::shared_ptr<TrueTypeFont>> fonts;
        if (!desc.fontPath.empty()) {
            auto [font, fontErr] = loadTrueTypeFont(fs_, std::string{desc.fontPath});
            if (fontErr != nullptr) {
                return errors::wrap(std::move(fontErr), "failed to load a font file");
            }
            fonts.push_back(std::move(font));
        }
        fonts.push_back(latinFont);

        const std::vector<f32> fontSizes(fonts.size(), fontSize);
        auto [spriteFont, spriteFontErr] = createSpriteFont(graphicsDevice_, fonts, fontSizes, desc.lineSpacing, true);
        if (spriteFontErr != nullptr) {
            return errors::wrap(std::move(spriteFontErr), "failed to create SpriteFont");
        }

        Sample sample;
        sample.label = std::string{desc.label};
        sample.text = std::string{desc.text};
        sample.language = desc.language;
        sample.font = std::move(spriteFont);

        // NOTE: Rasterize every glyph up front so that wrapping can measure
        // with cached glyphs only, without touching the GPU.
        sample.font->prepareFonts(graphicsDevice_, sample.label + sample.text);
        samples_.push_back(std::move(sample));
    }

    return nullptr;
}

void TextWrapTest::wrapSamples(f32 wrapWidth)
{
    for (auto& sample : samples_) {
        const auto& font = sample.font;
        const auto measure = [&font](std::string_view text) -> f32 {
            return font->measureString(nullptr, std::string{text}).x * textScale;
        };
        sample.wrappedText = i18n::wrapText(measure, sample.text, wrapWidth, sample.language).wrappedText;
    }
    wrappedWidth_ = wrapWidth;
}

void TextWrapTest::update()
{
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();
    const auto gesture = getPrimaryGestureState(*gameHost_->getMouse(), gameHost_->getTouchscreen().get());

    // NOTE: Ignore presses on the header, where the Back button is.
    if (gesture.pressed && static_cast<f32>(gesture.position.y) > headerHeight) {
        isAnimating_ = false;
        const auto x = static_cast<f32>(gesture.position.x) - cellMargin;
        const auto range = std::max(1.0f, static_cast<f32>(clientBounds.width) - cellMargin * 2.0f);
        wrapRatio_ = std::clamp(x / range, minWrapRatio, 1.0f);
    }
    if (isAnimating_) {
        const auto time = static_cast<f32>(gameHost_->getClock()->getTotalGameTime().count());
        wrapRatio_ = minWrapRatio + (1.0f - minWrapRatio) * (0.5f + 0.5f * std::cos(time * 0.6f));
    }

    const auto wrapWidth = computeWrapWidth(clientBounds, wrapRatio_);
    if (std::abs(wrapWidth - wrappedWidth_) >= 0.5f) {
        wrapSamples(wrapWidth);
    }
}

void TextWrapTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();
    const auto pixelRatio = gameHost_->getWindow()->getPixelRatio();
    const auto textSDFParameters = computeSpriteFontSDFParameters(
        SpriteFontSDFDesc{.fontSize = fontSize, .effectiveScale = textScale * pixelRatio});
    const auto labelSDFParameters = computeSpriteFontSDFParameters(
        SpriteFontSDFDesc{.fontSize = fontSize, .effectiveScale = labelScale * pixelRatio});

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
        0.0f,
        100.0f);

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

    const auto wrapWidth = wrappedWidth_;
    const auto identity = Matrix3x2::createIdentity();

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    for (int index = 0; index < static_cast<int>(samples_.size()); ++index) {
        const auto cell = computeCellLayout(clientBounds, index);
        const auto bottom = cell.top - cell.height;
        const auto textLeft = cell.left + cellPadding;
        const auto textTop = cell.top - cellPadding - labelHeight;
        const auto wrapRight = textLeft + wrapWidth;

        primitiveBatch_->drawRectangle(identity, Vector2{cell.left, bottom}, cell.width, cell.height, Color{52, 55, 66, 255});
        primitiveBatch_->drawRectangle(identity, Vector2{textLeft, bottom + cellPadding}, wrapWidth, textTop - bottom - cellPadding, Color{64, 68, 84, 255});
        primitiveBatch_->drawLine(Vector2{wrapRight, bottom + cellPadding}, Vector2{wrapRight, textTop}, Color{255, 176, 64, 255}, 1.0f);
    }
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);
    for (int index = 0; index < static_cast<int>(samples_.size()); ++index) {
        const auto& sample = samples_[static_cast<std::size_t>(index)];
        const auto cell = computeCellLayout(clientBounds, index);
        const auto textLeft = cell.left + cellPadding;

        sample.font->draw(
            graphicsDevice_,
            *spriteBatch_,
            sample.label,
            Vector2{textLeft, cell.top - cellPadding * 0.5f},
            SpriteFontDrawParameters{
                .color = Color{160, 168, 190, 255},
                .fontSmoothing = labelSDFParameters.fontSmoothing,
                .fontWeight = labelSDFParameters.fontWeight,
                .originPivot = Vector2{0.0f, 1.0f},
                .scale = Vector2{labelScale, labelScale},
            });
        sample.font->draw(
            graphicsDevice_,
            *spriteBatch_,
            sample.wrappedText,
            Vector2{textLeft, cell.top - cellPadding - labelHeight},
            SpriteFontDrawParameters{
                .color = Color::createWhite(),
                .fontSmoothing = textSDFParameters.fontSmoothing,
                .fontWeight = textSDFParameters.fontWeight,
                .originPivot = Vector2{0.0f, 1.0f},
                .scale = Vector2{textScale, textScale},
            });
    }

    const auto hint = pomdog::format("Drag to change the wrap width: {} px", static_cast<int>(wrapWidth));
    hintFont_->draw(
        graphicsDevice_,
        *spriteBatch_,
        hint,
        Vector2{static_cast<f32>(clientBounds.width) * 0.5f - cellMargin, static_cast<f32>(clientBounds.height) * 0.5f - 12.0f},
        SpriteFontDrawParameters{
            .color = Color{200, 204, 214, 255},
            .fontSmoothing = labelSDFParameters.fontSmoothing,
            .fontWeight = labelSDFParameters.fontWeight,
            .originPivot = Vector2{1.0f, 1.0f},
            .scale = Vector2{labelScale, labelScale},
        });

    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();
    commandList_->close();

    commandQueue_->pushBackCommandList(commandList_);
}

} // namespace feature_showcase
