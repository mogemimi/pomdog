#include "distance_field_font_test.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/popup_menu.h"
#include "pomdog/experimental/gui/slider.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/toggle_switch.h"
#include "pomdog/experimental/gui/vertical_layout.h"

namespace feature_showcase {

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

    // NOTE: Setup GUI
    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto err = drawingContext_->initialize(graphicsDevice_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->getDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 240, 180);
        stackPanel->setPosition(Point2D{5, 260});
        stackPanel->setPadding(gui::Thickness{8, 2, 2, 8});
        hierarchy_->addChild(stackPanel);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 196, 180);
        verticalLayout->setLayoutSpacing(4);
        stackPanel->addChild(verticalLayout);

        // fontSmoothing slider
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Smoothing");
            row->addChild(label);
            auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 1.0);
            slider->setValue(static_cast<double>(fontSmoothing_));
            connect_(slider->ValueChanged, [this](double v) {
                fontSmoothing_ = static_cast<f32>(v);
                useAutomaticSDFParameters_ = false;
            });
            row->addChild(slider);
            row->setStretchFactor(label, 2);
            row->setStretchFactor(slider, 3);
        }
        // fontWeight slider
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Weight");
            row->addChild(label);
            auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 1.0);
            slider->setValue(static_cast<double>(fontWeight_));
            connect_(slider->ValueChanged, [this](double v) {
                fontWeight_ = static_cast<f32>(v);
                useAutomaticSDFParameters_ = false;
            });
            row->addChild(slider);
            row->setStretchFactor(label, 2);
            row->setStretchFactor(slider, 3);
        }
        // outlineWeight slider
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Outline");
            row->addChild(label);
            auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 1.0);
            slider->setValue(static_cast<double>(outlineWeight_));
            connect_(slider->ValueChanged, [this](double v) {
                outlineWeight_ = static_cast<f32>(v);
                useAutomaticSDFParameters_ = false;
            });
            row->addChild(slider);
            row->setStretchFactor(label, 2);
            row->setStretchFactor(slider, 3);
        }
        // fontScale slider
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Scale");
            row->addChild(label);
            auto slider = std::make_shared<gui::Slider>(dispatcher, 0.1, 4.0);
            slider->setValue(static_cast<double>(fontScale_));
            connect_(slider->ValueChanged, [this](double v) { fontScale_ = static_cast<f32>(v); });
            row->addChild(slider);
            row->setStretchFactor(label, 2);
            row->setStretchFactor(slider, 3);
        }
        // useOutline toggle
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Outline");
            row->addChild(label);
            auto toggle = std::make_shared<gui::ToggleSwitch>(dispatcher);
            toggle->setOn(useOutline_);
            connect_(toggle->Toggled, [this](bool isOn) { useOutline_ = isOn; });
            row->addChild(toggle);
        }
        // preset popup menu
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);

            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText("Preset");
            row->addChild(textBlock);

            auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
            popupMenu->addItem("Automatic, no outline");
            popupMenu->addItem("Automatic, outline");
            popupMenu->addItem("Sharp, no outline");
            popupMenu->addItem("Smooth, no outline");
            popupMenu->addItem("With black outline");
            popupMenu->addItem("With red outline, thick");
            connect_(popupMenu->CurrentIndexChanged, [this](int index) {
                if (index == 0) {
                    useAutomaticSDFParameters_ = true;
                    useOutline_ = false;
                }
                if (index == 1) {
                    useAutomaticSDFParameters_ = true;
                    useOutline_ = true;
                    outlineColor_ = Color::createBlack();
                }
                if (index == 2) {
                    useAutomaticSDFParameters_ = false;
                    fontSmoothing_ = 0.05f;
                    fontWeight_ = 0.55f;
                    outlineWeight_ = 0.5f;
                    useOutline_ = false;
                }
                if (index == 3) {
                    useAutomaticSDFParameters_ = false;
                    fontSmoothing_ = 0.25f;
                    fontWeight_ = 0.55f;
                    outlineWeight_ = 0.5f;
                    useOutline_ = false;
                }
                if (index == 4) {
                    useAutomaticSDFParameters_ = false;
                    fontSmoothing_ = 0.15f;
                    fontWeight_ = 0.85f;
                    outlineWeight_ = 0.5f;
                    outlineColor_ = Color::createBlack();
                    useOutline_ = true;
                }
                if (index == 5) {
                    useAutomaticSDFParameters_ = false;
                    fontSmoothing_ = 0.15f;
                    fontWeight_ = 0.85f;
                    outlineWeight_ = 0.3f;
                    outlineColor_ = Color::createRed();
                    useOutline_ = true;
                }
            });
            row->addChild(popupMenu);
        }
    }

    return nullptr;
}

void DistanceFieldFontTest::update()
{
    hierarchy_->update();
    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        fontScale_ = std::clamp(fontScale_ + static_cast<f32>(mouse->getScrollDeltaY() * 0.27), 0.1f, 4.0f);

        hierarchy_->touch(*mouse, gameHost_->getTouchscreen().get());
    }
    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void DistanceFieldFontTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();

    // NOTE: The text, guide lines, and GUI are positioned in logical pixels, so
    // the projection, view matrix, and DrawingContext viewport use the logical
    // client size. The viewport below stays in physical pixels.
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

    // NOTE: Draw GUI overlay
    {
        gpu::RenderPass guiPass;
        guiPass.renderTargets[0] = {nullptr, std::nullopt};
        guiPass.viewport = viewport;
        guiPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(guiPass));
    }
    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<f32>(-clientBounds.width) * 0.5f,
        static_cast<f32>(-clientBounds.height) * 0.5f,
        0.0f});

    drawingContext_->reset(clientBounds.width, clientBounds.height, gameHost_->getWindow()->getPixelRatio());
    drawingContext_->beginDraw(commandList_, viewMatrix * projectionMatrix);
    hierarchy_->draw(*drawingContext_);
    drawingContext_->endDraw();

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
