#include "sprite_batch_test.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/popup_menu.h"
#include "pomdog/experimental/gui/push_button.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/math/easing.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

SpriteBatchTest::SpriteBatchTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
SpriteBatchTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
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
        spriteBatchNoOptimization_ = std::move(p);
    }
    if (auto [p, err] = createSpriteBatch(graphicsDevice_, std::nullopt, SpriteBatchOptimizationKind::SortedSingleTexture); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatchSortedSingleTexture_ = std::move(p);
    }

    // NOTE: Load PNG texture.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.2);

    {
        sprites_.push_back(SpriteInstance{
            .position = Vector2{0.0f, 0.0f},
            .scale = Vector2{2.0f, 2.0f},
            .color = Color::createWhite(),
        });
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
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText("Optimization");
            row->addChild(textBlock);
            auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
            popupMenu->addItem("Not Specified");
            popupMenu->addItem("Sorted Single Texture");
            connect_(popupMenu->CurrentIndexChanged, [this, p = popupMenu.get()](int index) {
                if (index == 0) {
                    optimizationKind_ = SpriteBatchOptimizationKind::NotSpecified;
                }
                if (index == 1) {
                    optimizationKind_ = SpriteBatchOptimizationKind::SortedSingleTexture;
                }
            });
            row->addChild(popupMenu);
        }
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);

            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText("Sort Mode");
            row->addChild(textBlock);

            auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
            popupMenu->addItem("None");
            popupMenu->addItem("Texture");
            popupMenu->addItem("BackToFront");
            popupMenu->addItem("FrontToBack");
            connect_(popupMenu->CurrentIndexChanged, [this, p = popupMenu.get()](int index) {
                if (index == 0) {
                    sortMode_ = std::nullopt;
                }
                if (index == 1) {
                    sortMode_ = SpriteSortMode::Texture;
                }
                if (index == 2) {
                    sortMode_ = SpriteSortMode::BackToFront;
                }
                if (index == 3) {
                    sortMode_ = SpriteSortMode::FrontToBack;
                }
            });
            row->addChild(popupMenu);
        }
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);
            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText("Origin Pivot");
            row->addChild(textBlock);
            auto popupMenu = std::make_shared<gui::PopupMenu>(dispatcher);
            popupMenu->addItem("BottomLeft");
            popupMenu->addItem("Center");
            popupMenu->addItem("TopRight");
            popupMenu->addItem("Custom (-1, -1)");
            connect_(popupMenu->CurrentIndexChanged, [this, p = popupMenu.get()](int index) {
                if (index == 0) {
                    originPivot_ = Vector2{0.0f, 0.0f};
                }
                if (index == 1) {
                    originPivot_ = Vector2{0.5f, 0.5f};
                }
                if (index == 2) {
                    originPivot_ = Vector2{1.0f, 1.0f};
                }
                if (index == 3) {
                    originPivot_ = Vector2{-1.0f, -1.0f};
                }
            });
            row->addChild(popupMenu);
        }
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 190, 18);
            verticalLayout->addChild(row);

            auto textBlock = std::make_shared<gui::TextBlock>(dispatcher);
            textBlock->setColor(Color::createWhite());
            textBlock->setText("Add Sprite +100");
            row->addChild(textBlock);

            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            button->setText("Add Sprite");
            connect_(button->Click, [this]() {
                std::mt19937 random(std::random_device{}());
                std::uniform_real_distribution<f32> scaleDist(1.0f, 2.0f);
                std::uniform_int_distribution<int> colorDist(160, 255);

                for (int i = 0; i < 100; ++i) {
                    sprites_.push_back(SpriteInstance{
                        .position = Vector2{
                            std::uniform_real_distribution<f32>(-400.0f, 400.0f)(random),
                            std::uniform_real_distribution<f32>(-300.0f, 300.0f)(random),
                        },
                        .scale = Vector2{scaleDist(random), scaleDist(random)},
                        .color = Color{
                            static_cast<u8>(colorDist(random)),
                            static_cast<u8>(colorDist(random)),
                            static_cast<u8>(colorDist(random)),
                            255,
                        },
                    });
                }
            });
            row->addChild(button);
        }
    }

    return nullptr;
}

void SpriteBatchTest::update()
{
    hierarchy_->update();
    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        const bool leftDown = mouse->isButtonDown(MouseButtons::Left);
        if (leftDown && !wasLeftMouseDown_) {
            const auto window = gameHost_->getWindow();
            const auto clientBounds = window->getClientBounds();
            auto pos = mouse->getPosition();
            pos.x = pos.x - (clientBounds.width / 2);
            pos.y = -pos.y + (clientBounds.height / 2);

            std::mt19937 random(std::random_device{}());
            std::uniform_real_distribution<f32> scaleDist(1.0f, 2.0f);
            std::uniform_int_distribution<i32> colorDist(160, 255);

            const auto scale = scaleDist(random);

            SpriteInstance sprite;
            sprite.position = math::toVector2(pos);
            sprite.scale.x = scale;
            sprite.scale.y = scale;
            sprite.color.r = static_cast<u8>(colorDist(random));
            sprite.color.g = static_cast<u8>(colorDist(random));
            sprite.color.b = static_cast<u8>(colorDist(random));
            sprite.color.a = 255;
            sprites_.push_back(std::move(sprite));
        }
        wasLeftMouseDown_ = leftDown;

        hierarchy_->touch(*mouse);
    }
    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void SpriteBatchTest::draw()
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

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    const auto t = static_cast<float>(timer_->getTotalTime().count());

    // Drawing line
    const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
    const auto h = static_cast<f32>(presentationParameters.backBufferHeight);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    auto spriteBatch = spriteBatchNoOptimization_;
    if (optimizationKind_ == SpriteBatchOptimizationKind::SortedSingleTexture) {
        spriteBatch = spriteBatchSortedSingleTexture_;
    }

    spriteBatch->reset();
    spriteBatch->setTransform(projectionMatrix);

    {
        f32 layerDepth = 0.0f;
        for (const auto& sprite : sprites_) {
            spriteBatch->draw(
                texture_,
                sprite.position,
                Rect2D{0, 0, texture_->getWidth(), texture_->getHeight()},
                SpriteBatchDrawParameters{
                    .color = sprite.color,
                    .rotation = math::TwoPi<f32> * math::easeInOutSine(t),
                    .originPivot = originPivot_,
                    .scale = sprite.scale,
                    .layerDepth = layerDepth,
                });

            layerDepth += 0.0001f;
        }
    }

    if (sortMode_.has_value()) {
        spriteBatch->sort(*sortMode_);
    }

    spriteBatch->flush(commandList_, spritePipeline_);
    spriteBatch->submit(graphicsDevice_);
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
