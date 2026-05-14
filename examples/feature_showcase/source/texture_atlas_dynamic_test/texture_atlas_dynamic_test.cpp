#include "texture_atlas_dynamic_test.h"
#include "pomdog/content/texture_loader.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/popup_menu.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/experimental/image/image.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

namespace {

/// Builds a solid-color 64x64 Image for atlas demo purposes.
[[nodiscard]] std::shared_ptr<pomdog::Image>
makeColorImage(i32 w, i32 h, pomdog::Color color)
{
    auto img = std::make_shared<pomdog::Image>(w, h);
    img->Fill(pomdog::Color{0, 0, 0, 0});
    for (i32 y = 2; y < h - 2; ++y) {
        for (i32 x = 2; x < w - 2; ++x) {
            img->SetPixel(x, y, color);
        }
    }
    return img;
}

} // namespace

TextureAtlasDynamicTest::TextureAtlasDynamicTest(
    const std::shared_ptr<GameHost>& gameHostIn,
    const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
TextureAtlasDynamicTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
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
            gpu::SamplerDesc::createPointWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Sprite);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }

    if (auto [p, err] = createSpriteBatch(graphicsDevice_, std::nullopt, SpriteBatchOptimizationKind::NotSpecified); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    // NOTE: Build a TextureAtlasDynamic from programmatically generated colored sprites.
    dynamicAtlas_ = createTextureAtlasDynamic();

    struct SpriteDesc final {
        std::string name;
        int w;
        int h;
        Color color;
    };

    const std::vector<SpriteDesc> sprites = {
        {"red_32", 32, 32, Color{220, 50, 50, 255}},
        {"green_64", 64, 64, Color{50, 200, 50, 255}},
        {"blue_48", 48, 48, Color{50, 80, 220, 255}},
        {"yellow_24", 24, 24, Color{230, 200, 40, 255}},
        {"cyan_96", 96, 96, Color{40, 200, 210, 255}},
        {"magenta_16", 16, 16, Color{200, 40, 200, 255}},
    };

    for (const auto& s : sprites) {
        auto img = makeColorImage(s.w, s.h, s.color);
        dynamicAtlas_->add(s.name, img, true);
        regionNames_.push_back(s.name);
    }

    if (auto err = dynamicAtlas_->build(graphicsDevice_, 256, 256, 2); err != nullptr) {
        return errors::wrap(std::move(err), "TextureAtlasDynamic::build() failed");
    }

    // NOTE: Cache all regions for rendering.
    for (u16 i = 0; i < static_cast<u16>(regionNames_.size()); ++i) {
        spriteRegions_.push_back(dynamicAtlas_->getRegion(i));
    }

    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto err = drawingContext_->initialize(graphicsDevice_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->getDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 240, 60);
        stackPanel->setPosition(Point2D{5, 200});
        stackPanel->setPadding(gui::Thickness{8, 2, 2, 8});
        hierarchy_->addChild(stackPanel);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 196, 60);
        verticalLayout->setLayoutSpacing(4);
        stackPanel->addChild(verticalLayout);

        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText("Sprite Region");
            row->addChild(textBlock);
            auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
            for (const auto& name : regionNames_) {
                popupMenu->addItem(name);
            }
            connect_(popupMenu->CurrentIndexChanged, [this](int index) {
                selectedRegion_ = index;
            });
            row->addChild(popupMenu);
        }
    }

    return nullptr;
}

void TextureAtlasDynamicTest::update()
{
    hierarchy_->update();
    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->touch(*mouse);
    }
    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void TextureAtlasDynamicTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
    const auto h = static_cast<f32>(presentationParameters.backBufferHeight);

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 100}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 100}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    if (dynamicAtlas_ && dynamicAtlas_->getTexture()) {
        auto tex = dynamicAtlas_->getTexture();

        spriteBatch_->reset();
        spriteBatch_->setTransform(projectionMatrix);

        // NOTE: Draw the full atlas at top-left
        spriteBatch_->draw(
            tex,
            Vector2{-w * 0.5f + 10.0f + 128.0f, h * 0.5f - 10.0f - 128.0f},
            Rect2D{0, 0, tex->getWidth(), tex->getHeight()},
            SpriteBatchDrawParameters{
                .color = Color::createWhite(),
                .scale = Vector2{1.0f, 1.0f},
            });

        // NOTE: Draw the selected sprite region at centre
        if (!spriteRegions_.empty()) {
            const auto idx = static_cast<std::size_t>(selectedRegion_) % spriteRegions_.size();
            const auto& region = spriteRegions_[idx];
            spriteBatch_->draw(
                tex,
                Vector2{100.0f, 0.0f},
                Rect2D{
                    region.subrectX,
                    region.subrectY,
                    region.subrectWidth,
                    region.subrectHeight,
                },
                SpriteBatchDrawParameters{
                    .color = Color::createWhite(),
                    .scale = Vector2{3.0f, 3.0f},
                });
        }

        spriteBatch_->flush(commandList_, spritePipeline_);
        spriteBatch_->submit(graphicsDevice_);
    }

    commandList_->endRenderPass();

    // GUI overlay
    {
        gpu::RenderPass guiPass;
        guiPass.renderTargets[0] = {nullptr, std::nullopt};
        guiPass.viewport = viewport;
        guiPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(guiPass));
    }
    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<f32>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<f32>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext_->reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
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
