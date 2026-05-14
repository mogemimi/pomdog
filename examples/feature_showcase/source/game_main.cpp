#include "game_main.h"
#include "animation_graph_test/animation_graph_test.h"
#include "audio_clip_test/audio_clip_test.h"
#include "basic_effect_test/basic_effect_test.h"
#include "beam2d_test/beam2d_test.h"
#include "billboard_batch_test/billboard_batch_test.h"
#include "bug_issue_49_test/bug_issue_49_test.h"
#include "display_settings_test/display_settings_test.h"
#include "distance_field_font_test/distance_field_font_test.h"
#include "editor_gui_test/editor_gui_test.h"
#include "gamepad_test/gamepad_test.h"
#include "gltf_model_test/gltf_model_test.h"
#include "gui_splitter_test/gui_splitter_test.h"
#include "hardware_instancing_test/hardware_instancing_test.h"
#include "image_effects_test/image_effects_test.h"
#include "line_batch_test/line_batch_test.h"
#include "multi_render_target_test/multi_render_target_test.h"
#include "particle2d_test/particle2d_test.h"
#include "particle3d_test/particle3d_test.h"
#include "particle_clip_loader_test/particle_clip_loader_test.h"
#include "polyline_drawing_test/polyline_drawing_test.h"
#include "primitive_batch_test/primitive_batch_test.h"
#include "skeletal2d_test/skeletal2d_test.h"
#include "skinning2d_test/skinning2d_test.h"
#include "sprite_batch_effect_test/sprite_batch_effect_test.h"
#include "sprite_batch_test/sprite_batch_test.h"
#include "sprite_font_test/sprite_font_test.h"
#include "sprite_line_test/sprite_line_test.h"
#include "svg_decode_test/svg_decode_test.h"
#include "texture2d_loader_test/texture2d_loader_test.h"
#include "texture_atlas_dynamic_test/texture_atlas_dynamic_test.h"
#include "texture_atlas_static_test/texture_atlas_static_test.h"
#include "voxel_model_test/voxel_model_test.h"
#include "pomdog/utility/string_format.h"

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "http_client_test/http_client_test.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

GameMain::GameMain(std::shared_ptr<vfs::FileSystemContext> fs)
    : fs_(std::move(fs))
{
}

std::unique_ptr<Error>
GameMain::initialize(const std::shared_ptr<GameHost>& gameHostIn)
{
    gameHost_ = gameHostIn;
    window_ = gameHostIn->getWindow();
    graphicsDevice_ = gameHostIn->getGraphicsDevice();
    clock_ = gameHostIn->getClock();
    commandQueue_ = gameHostIn->getCommandQueue();

    window_->requestTitle("Feature Showcase");
    window_->requestAllowUserResizing(true);

    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load TrueType font");
    }

    constexpr bool useSDF = true;

    if (auto [p, err] = createSpriteFont(graphicsDevice_, font, 20.0f, 20.0f, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont_ = std::move(p);
    }
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            std::nullopt,
            std::nullopt,
            gpu::SamplerDesc::createLinearClamp(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::DistanceFieldWithOutline);
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

    spriteFont_->prepareFonts(
        graphicsDevice_,
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");

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
    timer_ = std::make_shared<Timer>(clock_);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(1.0);

    buttons_.emplace_back("DisplaySettings Test", [this] {
        window_->requestTitle("Feature Showcase > DisplaySettings Test");
        subGame_ = std::make_shared<feature_showcase::DisplaySettingsTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("EditorGUI Test", [this] {
        window_->requestTitle("Feature Showcase > EditorGUI Test");
        subGame_ = std::make_shared<feature_showcase::EditorGUITest>(gameHost_, fs_);
    });
    buttons_.emplace_back("GUISplitter Test", [this] {
        window_->requestTitle("Feature Showcase > GUISplitter Test");
        subGame_ = std::make_shared<feature_showcase::GUISplitterTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("LineBatch Test", [this] {
        window_->requestTitle("Feature Showcase > LineBatch Test");
        subGame_ = std::make_shared<feature_showcase::LineBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("PolylineDrawing Test", [this] {
        window_->requestTitle("Feature Showcase > PolylineDrawing Test");
        subGame_ = std::make_shared<feature_showcase::PolylineDrawingTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("PrimitiveBatch Test", [this] {
        window_->requestTitle("Feature Showcase > PrimitiveBatch Test");
        subGame_ = std::make_shared<feature_showcase::PrimitiveBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("BasicEffect Test", [this] {
        window_->requestTitle("Feature Showcase > BasicEffect Test");
        subGame_ = std::make_shared<feature_showcase::BasicEffectTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("GLTFModel Test", [this] {
        window_->requestTitle("Feature Showcase > GLTFModel Test");
        subGame_ = std::make_shared<feature_showcase::GLTFModelTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("HardwareInstancing Test", [this] {
        window_->requestTitle("Feature Showcase > HardwareInstancing Test");
        subGame_ = std::make_shared<feature_showcase::HardwareInstancingTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("MultiRenderTarget Test", [this] {
        window_->requestTitle("Feature Showcase > MultiRenderTarget Test");
        subGame_ = std::make_shared<feature_showcase::MultiRenderTargetTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteBatch Test", [this] {
        window_->requestTitle("Feature Showcase > SpriteBatch Test");
        subGame_ = std::make_shared<feature_showcase::SpriteBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteBatchEffect Test", [this] {
        window_->requestTitle("Feature Showcase > SpriteBatchEffect Test");
        subGame_ = std::make_shared<feature_showcase::SpriteBatchEffectTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteFont Test", [this] {
        window_->requestTitle("Feature Showcase > SpriteFont Test");
        subGame_ = std::make_shared<feature_showcase::SpriteFontTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("DistanceFieldFont Test", [this] {
        window_->requestTitle("Feature Showcase > DistanceFieldFont Test");
        subGame_ = std::make_shared<feature_showcase::DistanceFieldFontTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteLine Test", [this] {
        window_->requestTitle("Feature Showcase > SpriteLine Test");
        subGame_ = std::make_shared<feature_showcase::SpriteLineTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("BillboardBatch Test", [this] {
        window_->requestTitle("Feature Showcase > BillboardBatch Test");
        subGame_ = std::make_shared<feature_showcase::BillboardBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("ImageEffects Test", [this] {
        window_->requestTitle("Feature Showcase > ImageEffects Test");
        subGame_ = std::make_shared<feature_showcase::ImageEffectsTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Particle2D Test", [this] {
        window_->requestTitle("Feature Showcase > Particle2D Test");
        subGame_ = std::make_shared<feature_showcase::Particle2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("ParticleClipLoader Test", [this] {
        window_->requestTitle("Feature Showcase > ParticleClipLoader Test");
        subGame_ = std::make_shared<feature_showcase::ParticleClipLoaderTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Particle3D Test", [this] {
        window_->requestTitle("Feature Showcase > Particle3D Test");
        subGame_ = std::make_shared<feature_showcase::Particle3DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Beam2D Test", [this] {
        window_->requestTitle("Feature Showcase > Beam2D Test");
        subGame_ = std::make_shared<feature_showcase::Beam2DTest>(gameHost_, fs_);
    });
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    buttons_.emplace_back("HTTPClient Test", [this] {
        window_->requestTitle("Feature Showcase > HTTPClient Test");
        subGame_ = std::make_shared<feature_showcase::HTTPClientTest>(gameHost_, fs_);
    });
#endif
    buttons_.emplace_back("AudioClip Test", [this] {
        window_->requestTitle("Feature Showcase > AudioClip Test");
        subGame_ = std::make_shared<feature_showcase::AudioClipTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Texture2DLoader Test", [this] {
        window_->requestTitle("Feature Showcase > Texture2DLoader Test");
        subGame_ = std::make_shared<feature_showcase::Texture2DLoaderTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("TextureAtlasDynamic Test", [this] {
        window_->requestTitle("Feature Showcase > TextureAtlasDynamic Test");
        subGame_ = std::make_shared<feature_showcase::TextureAtlasDynamicTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("TextureAtlasStatic Test", [this] {
        window_->requestTitle("Feature Showcase > TextureAtlasStatic Test");
        subGame_ = std::make_shared<feature_showcase::TextureAtlasStaticTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SVGDecode Test", [this] {
        window_->requestTitle("Feature Showcase > SVGDecode Test");
        subGame_ = std::make_shared<feature_showcase::SVGDecodeTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("VoxelModel Test", [this] {
        window_->requestTitle("Feature Showcase > VoxelModel Test");
        subGame_ = std::make_shared<feature_showcase::VoxelModelTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Gamepad Test", [this] {
        window_->requestTitle("Feature Showcase > Gamepad Test");
        subGame_ = std::make_shared<feature_showcase::GamepadTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Skeletal2D Test", [this] {
        window_->requestTitle("Feature Showcase > Skeletal2D Test");
        subGame_ = std::make_shared<feature_showcase::Skeletal2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Skinning2D Test", [this] {
        window_->requestTitle("Feature Showcase > Skinning2D Test");
        subGame_ = std::make_shared<feature_showcase::Skinning2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("AnimationGraph Test", [this] {
        window_->requestTitle("Feature Showcase > AnimationGraph Test");
        subGame_ = std::make_shared<feature_showcase::AnimationGraphTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Bug Issue #49", [this] {
        window_->requestTitle("Feature Showcase > Bug Issue #49 Test");
        subGame_ = std::make_shared<feature_showcase::BugIssue49Test>(gameHost_, fs_);
    });

    hudButtons_.emplace_back("Back", [this] {
        window_->requestTitle("Feature Showcase");
        subGame_.reset();
    });

    auto mouse = gameHost_->getMouse();

    fpsTimer_ = std::make_shared<Timer>(clock_);
    fpsTimer_->setInterval(std::chrono::milliseconds(150));
    connect_(fpsTimer_->elapsed, [this] {
        footerString_ = pomdog::format("{:.2f} fps", clock_->getFrameRate());
    });

    return nullptr;
}

void GameMain::update()
{
    if (subGame_) {
        subGame_->update();
    }

    updateMenuLayout();

    const auto mouse = gameHost_->getMouse();
    const auto clientBounds = window_->getClientBounds();
    auto position = mouse->getPosition();
    position.y = clientBounds.height - position.y;

    if (subGame_) {
        for (auto& button : hudButtons_) {
            button.Selected = button.Rect.contains(position);
        }
    }
    else {
        for (auto& button : buttons_) {
            button.Selected = button.Rect.contains(position);
        }
    }

    {
        const auto currentScrollWheel = mouse->getScrollY();
        const auto delta = currentScrollWheel - prevScrollWheel_;
        prevScrollWheel_ = currentScrollWheel;
        // NOTE: Scroll is normalized to 1.0 per notch across all platforms.
        constexpr f64 scrollSpeed = 20.0;
        scrollY_ = std::clamp(scrollY_ + delta * scrollSpeed, -670.0, 0.0);
    }

    const bool leftDown = mouse->isButtonDown(MouseButtons::Left);
    if (leftDown && !wasLeftMouseDown_) {
        if (subGame_) {
            for (auto& button : hudButtons_) {
                if (button.Selected) {
                    button.OnClicked();
                }
            }
        }
        else {
            for (auto& button : buttons_) {
                if (button.Selected) {
                    button.OnClicked();
                    if (subGame_) {
                        if (auto err = subGame_->initialize(gameHost_); err != nullptr) {
                            Log::Critical("Error", "failed to initialize game: " + err->toString());
                            subGame_.reset();
                        }
                    }
                }
            }
        }
    }
    wasLeftMouseDown_ = leftDown;
}

void GameMain::updateMenuLayout()
{
    const auto mouse = gameHost_->getMouse();
    const auto clientBounds = window_->getClientBounds();

    {
        int y = clientBounds.height - 36 - static_cast<int>(scrollY_);
        constexpr int buttonVerticalMargin = 5;

        for (auto& button : buttons_) {
            button.Rect.x = 10;
            button.Rect.y = y;
            button.Rect.width = 400;
            button.Rect.height = 28;
            y -= (button.Rect.height + buttonVerticalMargin);
        }
    }
    {
        int y = clientBounds.height - 36;
        constexpr int buttonVerticalMargin = 5;

        for (auto& button : hudButtons_) {
            button.Rect.x = 10;
            button.Rect.y = y;
            button.Rect.width = 80;
            button.Rect.height = 28;
            y -= (button.Rect.height + buttonVerticalMargin);
        }
    }
}

void GameMain::draw()
{
    commandQueue_->reset();

    if (subGame_) {
        subGame_->draw();
    }

    drawMenu();
    commandQueue_->pushBackCommandList(commandList_);

    commandQueue_->executeCommandLists();
    commandQueue_->present();
}

void GameMain::drawMenu()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    std::optional<Vector4> clearColor;
    if (!subGame_) {
        clearColor = Color{60, 60, 60, 255}.toVector4();
    }

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, clearColor};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<f32>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<f32>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    auto viewProjection = viewMatrix * projectionMatrix;

    auto drawButton = [this](const Button& button) {
        {
            Color color = Color{121, 120, 118, 255};
            if (button.Selected) {
                color = math::lerp(
                    Color{160, 160, 160, 255},
                    Color{191, 190, 180, 255},
                    0.5f + 0.5f * std::cos(math::TwoPi<f32> * static_cast<f32>(timer_->getTotalTime().count())));
            }

            primitiveBatch_->drawRectangle(
                Vector2{static_cast<f32>(button.Rect.x), static_cast<f32>(button.Rect.y)},
                static_cast<f32>(button.Rect.width),
                static_cast<f32>(button.Rect.height),
                Vector2{0.0f, 0.0f}, color);
        }
        {
            Vector2 position = {static_cast<f32>(button.Rect.x), static_cast<f32>(button.Rect.y)};

            Color color = Color{221, 220, 218, 255};
            if (button.Selected) {
                color = Color::createWhite();
            }

            position.x += 10.0f;
            position.y += 9.0f;

            spriteFont_->draw(
                graphicsDevice_,
                *spriteBatch_,
                button.Text,
                position,
                color,
                0.0f,
                Vector2{0.0f, 0.0f},
                1.0f);
        }
    };

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(viewProjection);
    spriteBatch_->reset();
    spriteBatch_->setTransform(
        viewProjection,
        0.338f,
        0.426f,
        Color{34, 31, 29, 255},
        0.319f);
    if (subGame_) {
        for (const auto& button : hudButtons_) {
            drawButton(button);
        }

        spriteFont_->draw(
            graphicsDevice_,
            *spriteBatch_,
            footerString_,
            Vector2{static_cast<f32>(viewport.width) - 8.0f, 8.0f},
            Color::createWhite(),
            0.0f,
            Vector2{1.0f, 0.0f},
            1.0f);
    }
    else {
        for (const auto& button : buttons_) {
            drawButton(button);
        }
    }
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);
    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();
    commandList_->close();
}

} // namespace feature_showcase
