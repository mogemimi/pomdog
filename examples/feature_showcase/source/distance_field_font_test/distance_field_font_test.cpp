#include "distance_field_font_test.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

constexpr std::array<std::string_view, 6> presetNames = {{
    "Automatic, no outline",
    "Automatic, outline",
    "Sharp, no outline",
    "Smooth, no outline",
    "With black outline",
    "With red outline, thick",
}};

} // namespace

DistanceFieldFontTest::DistanceFieldFontTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
DistanceFieldFontTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
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

    // NOTE: Create SpritePipeline with DistanceField shader
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
        return errors::wrap(std::move(err), "failed to create SpritePipeline (DistanceField)");
    }
    else {
        spritePipelineDF_ = std::move(p);
    }

    // NOTE: Create SpritePipeline with DistanceFieldWithOutline shader
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceFieldWithOutline);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline (DistanceFieldWithOutline)");
    }
    else {
        spritePipelineDFOutline_ = std::move(p);
    }

    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    // NOTE: Load TrueType font and create SpriteFont with SDF enabled
    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load TrueType font");
    }

    constexpr bool useSDF = true;
    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 32.0f, 24.0f, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }

    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    return nullptr;
}

void DistanceFieldFontTest::applyPreset(i32 index) noexcept
{
    switch (index) {
    case 0:
        useAutomaticSDFParameters_ = true;
        useOutline_ = false;
        break;
    case 1:
        useAutomaticSDFParameters_ = true;
        useOutline_ = true;
        outlineColor_ = Color::createBlack();
        break;
    case 2:
        useAutomaticSDFParameters_ = false;
        fontSmoothing_ = 0.05f;
        fontWeight_ = 0.55f;
        outlineWeight_ = 0.5f;
        useOutline_ = false;
        break;
    case 3:
        useAutomaticSDFParameters_ = false;
        fontSmoothing_ = 0.25f;
        fontWeight_ = 0.55f;
        outlineWeight_ = 0.5f;
        useOutline_ = false;
        break;
    case 4:
        useAutomaticSDFParameters_ = false;
        fontSmoothing_ = 0.15f;
        fontWeight_ = 0.85f;
        outlineWeight_ = 0.5f;
        outlineColor_ = Color::createBlack();
        useOutline_ = true;
        break;
    case 5:
        useAutomaticSDFParameters_ = false;
        fontSmoothing_ = 0.15f;
        fontWeight_ = 0.85f;
        outlineWeight_ = 0.3f;
        outlineColor_ = Color::createRed();
        useOutline_ = true;
        break;
    default:
        break;
    }
}

void DistanceFieldFontTest::buildUI(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{300.0f, 260.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "sdf"_key, "SDF Parameters", options);
    if (!window.isOpen()) {
        return;
    }

    // NOTE: Moving a parameter switches to manual parameters.
    if (devgui::sliderFloat(ui, "smoothing"_key, "Smoothing", fontSmoothing_, 0.0f, 1.0f).changed) {
        useAutomaticSDFParameters_ = false;
    }
    if (devgui::sliderFloat(ui, "weight"_key, "Weight", fontWeight_, 0.0f, 1.0f).changed) {
        useAutomaticSDFParameters_ = false;
    }
    if (devgui::sliderFloat(ui, "outlineWeight"_key, "Outline Weight", outlineWeight_, 0.0f, 1.0f).changed) {
        useAutomaticSDFParameters_ = false;
    }
    devgui::sliderFloat(ui, "scale"_key, "Scale", fontScale_, 0.1f, 4.0f);
    devgui::checkbox(ui, "outline"_key, "Outline", useOutline_);
    if (devgui::combo(ui, "preset"_key, "Preset", presetIndex_, presetNames).changed) {
        applyPreset(presetIndex_);
    }
}

void DistanceFieldFontTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });

    // NOTE: The wheel scales the text, unless it scrolled the panel.
    if (auto mouse = gameHost_->getMouse(); (mouse != nullptr) && !overlay_.getInputCapture().pointer) {
        fontScale_ = std::clamp(fontScale_ + static_cast<f32>(mouse->getScrollDeltaY() * 0.27), 0.1f, 4.0f);
    }
}

void DistanceFieldFontTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();

    // NOTE: The text and guide lines are positioned in logical pixels, so the
    // projection uses the logical client size. The viewport below stays in
    // physical pixels.
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
        0.0f,
        100.0f);

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    // NOTE: Draw guide lines
    const auto w = static_cast<f32>(clientBounds.width);
    const auto h = static_cast<f32>(clientBounds.height);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    // NOTE: Draw text with distance field font
    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);

    constexpr auto sampleText = "Hello, SDF Fonts!";
    const auto paramText = useAutomaticSDFParameters_
                               ? "Automatic SDF parameters, scroll to scale"
                               : "Manual SDF parameters, scroll to scale";
    const auto pixelRatio = gameHost_->getWindow()->getPixelRatio();
    const auto getSDFParameters = [this, pixelRatio](f32 drawScale) {
        if (useAutomaticSDFParameters_) {
            return computeSpriteFontSDFParameters(SpriteFontSDFDesc{
                .fontSize = 32.0f,
                .effectiveScale = drawScale * pixelRatio,
                .outlineThickness = useOutline_ ? 1.5f : 0.0f,
            });
        }
        return SpriteFontSDFParameters{
            .fontSmoothing = fontSmoothing_,
            .fontWeight = fontWeight_,
            .outlineWeight = outlineWeight_,
        };
    };
    const auto mainSDFParameters = getSDFParameters(fontScale_);
    const auto smallSDFParameters = getSDFParameters(fontScale_ * 0.7f);
    const auto labelSDFParameters = getSDFParameters(0.6f);

    spriteFont_->draw(
        graphicsDevice_,
        *spriteBatch_,
        sampleText,
        Vector2{0.0f, 80.0f},
        SpriteFontDrawParameters{
            .color = Color::createWhite(),
            .outlineColor = outlineColor_,
            .fontSmoothing = mainSDFParameters.fontSmoothing,
            .fontWeight = mainSDFParameters.fontWeight,
            .outlineWeight = mainSDFParameters.outlineWeight,
            .originPivot = Vector2{0.5f, 0.5f},
            .scale = Vector2{fontScale_, fontScale_},
        });

    spriteFont_->draw(
        graphicsDevice_,
        *spriteBatch_,
        sampleText,
        Vector2{0.0f, 20.0f},
        SpriteFontDrawParameters{
            .color = Color::createYellow(),
            .outlineColor = outlineColor_,
            .fontSmoothing = smallSDFParameters.fontSmoothing,
            .fontWeight = smallSDFParameters.fontWeight,
            .outlineWeight = smallSDFParameters.outlineWeight,
            .originPivot = Vector2{0.5f, 0.5f},
            .scale = Vector2{fontScale_ * 0.7f, fontScale_ * 0.7f},
        });

    spriteFont_->draw(
        graphicsDevice_,
        *spriteBatch_,
        paramText,
        Vector2{0.0f, -40.0f},
        SpriteFontDrawParameters{
            .color = Color{200, 200, 200, 200},
            .outlineColor = outlineColor_,
            .fontSmoothing = labelSDFParameters.fontSmoothing,
            .fontWeight = labelSDFParameters.fontWeight,
            .outlineWeight = labelSDFParameters.outlineWeight,
            .originPivot = Vector2{0.5f, 0.5f},
            .scale = Vector2{0.6f, 0.6f},
        });

    if (useOutline_) {
        spriteBatch_->flush(commandList_, spritePipelineDFOutline_);
    }
    else {
        spriteBatch_->flush(commandList_, spritePipelineDF_);
    }
    spriteBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();

    // NOTE: Draw the panel over the scene.
    {
        gpu::RenderPass overlayPass;
        overlayPass.renderTargets[0] = {nullptr, std::nullopt};
        overlayPass.viewport = viewport;
        overlayPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(overlayPass));
    }
    overlay_.draw(commandList_);
    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
