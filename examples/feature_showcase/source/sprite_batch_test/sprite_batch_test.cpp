#include "sprite_batch_test.h"
#include "pomdog/math/easing.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
#include <random>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

constexpr std::array<std::string_view, 2> optimizationNames = {{"Not Specified", "Sorted Single Texture"}};
constexpr std::array<SpriteBatchOptimizationKind, 2> optimizationKinds = {{
    SpriteBatchOptimizationKind::NotSpecified,
    SpriteBatchOptimizationKind::SortedSingleTexture,
}};

constexpr std::array<std::string_view, 4> sortModeNames = {{"None", "Texture", "BackToFront", "FrontToBack"}};
constexpr std::array<std::optional<SpriteSortMode>, 4> sortModes = {{
    std::nullopt,
    SpriteSortMode::Texture,
    SpriteSortMode::BackToFront,
    SpriteSortMode::FrontToBack,
}};

constexpr std::array<std::string_view, 4> originPivotNames = {{"BottomLeft", "Center", "TopRight", "Custom (-1, -1)"}};
[[nodiscard]] Vector2
getOriginPivot(i32 index) noexcept
{
    switch (index) {
    case 1:
        return Vector2{0.5f, 0.5f};
    case 2:
        return Vector2{1.0f, 1.0f};
    case 3:
        return Vector2{-1.0f, -1.0f};
    default:
        return Vector2{0.0f, 0.0f};
    }
}

} // namespace

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
    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    return nullptr;
}

void SpriteBatchTest::addSprites()
{
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
}

void SpriteBatchTest::buildUI(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{300.0f, 200.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "spriteBatch"_key, "Sprite Batch", options);
    if (!window.isOpen()) {
        return;
    }

    if (devgui::combo(ui, "optimization"_key, "Optimization", optimizationIndex_, optimizationNames).changed) {
        optimizationKind_ = optimizationKinds[static_cast<std::size_t>(optimizationIndex_)];
    }
    if (devgui::combo(ui, "sortMode"_key, "Sort Mode", sortModeIndex_, sortModeNames).changed) {
        sortMode_ = sortModes[static_cast<std::size_t>(sortModeIndex_)];
    }
    if (devgui::combo(ui, "originPivot"_key, "Origin Pivot", originPivotIndex_, originPivotNames).changed) {
        originPivot_ = getOriginPivot(originPivotIndex_);
    }
    if (devgui::button(ui, "addSprites"_key, "Add 100 Sprites").clicked) {
        addSprites();
    }
    devgui::labelText(ui, "Sprites", pomdog::format("{}", sprites_.size()));
}

void SpriteBatchTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });

    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        const auto gesture = getPrimaryGestureState(*mouse, gameHost_->getTouchscreen().get());
        const bool leftDown = gesture.pressed;

        // NOTE: A press on the panel does not add a sprite.
        if (leftDown && !wasLeftMouseDown_ && !overlay_.getInputCapture().pointer) {
            const auto window = gameHost_->getWindow();
            const auto clientBounds = window->getClientBounds();
            auto pos = gesture.position;
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
    }
}

void SpriteBatchTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();

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
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
        0.0f,
        100.0f);

    const auto t = static_cast<float>(timer_->getTotalTime().count());

    // Drawing line
    const auto w = static_cast<f32>(clientBounds.width);
    const auto h = static_cast<f32>(clientBounds.height);
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
