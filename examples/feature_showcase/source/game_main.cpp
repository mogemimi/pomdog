#include "game_main.h"
#include "animation_graph_test/animation_graph_test.h"
#include "audio_clip_test/audio_clip_test.h"
#include "basic_effect_test/basic_effect_test.h"
#include "beam2d_test/beam2d_test.h"
#include "billboard_batch_test/billboard_batch_test.h"
#include "distance_field_font_test/distance_field_font_test.h"
#include "editor_gui_test/editor_gui_test.h"
#include "gltf_model_test/gltf_model_test.h"
#include "gui_splitter_test/gui_splitter_test.h"
#include "gamepad_test/gamepad_test.h"
#include "http_client_test/http_client_test.h"
#include "hardware_instancing_test/hardware_instancing_test.h"
#include "image_effects_test/image_effects_test.h"
#include "line_batch_test/line_batch_test.h"
#include "multi_render_target_test/multi_render_target_test.h"
#include "particle_clip_loader_test/particle_clip_loader_test.h"
#include "particle2d_test/particle2d_test.h"
#include "particle3d_test/particle3d_test.h"
#include "polyline_drawing_test/polyline_drawing_test.h"
#include "primitive_batch_test/primitive_batch_test.h"
#include "svg_decode_test/svg_decode_test.h"
#include "skeletal2d_test/skeletal2d_test.h"
#include "skinning2d_test/skinning2d_test.h"
#include "sprite_batch_test/sprite_batch_test.h"
#include "sprite_font_test/sprite_font_test.h"
#include "sprite_line_test/sprite_line_test.h"
#include "texture2d_loader_test/texture2d_loader_test.h"
#include "voxel_model_test/voxel_model_test.h"
#include <cmath>
#include <utility>

namespace feature_showcase {

GameMain::GameMain(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->getWindow())
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , assets(gameHostIn->getAssetManager())
    , clock(gameHostIn->getClock())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
GameMain::initialize()
{
    window->setTitle("Feature Showcase");
    window->setAllowUserResizing(true);
    commandList = std::get<0>(graphicsDevice->createCommandList());

    auto [font, fontErr] = assets->Load<TrueTypeFont>("Fonts/NotoSans/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        Log::Critical("Error", "failed to load a font file: " + fontErr->toString());
    }

    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 20.0f, 20.0f);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(1.0);

    buttons.emplace_back("EditorGUI Test", [this] {
        window->setTitle("Feature Showcase > EditorGUI Test");
        subGame = std::make_shared<feature_showcase::EditorGUITest>(gameHost);
    });
    buttons.emplace_back("GUISplitter Test", [this] {
        window->setTitle("Feature Showcase > GUISplitter Test");
        subGame = std::make_shared<feature_showcase::GUISplitterTest>(gameHost);
    });
    buttons.emplace_back("LineBatch Test", [this] {
        window->setTitle("Feature Showcase > LineBatch Test");
        subGame = std::make_shared<feature_showcase::LineBatchTest>(gameHost);
    });
    buttons.emplace_back("PolylineDrawing Test", [this] {
        window->setTitle("Feature Showcase > PolylineDrawing Test");
        subGame = std::make_shared<feature_showcase::PolylineDrawingTest>(gameHost);
    });
    buttons.emplace_back("PrimitiveBatch Test", [this] {
        window->setTitle("Feature Showcase > PrimitiveBatch Test");
        subGame = std::make_shared<feature_showcase::PrimitiveBatchTest>(gameHost);
    });
    buttons.emplace_back("BasicEffect Test", [this] {
        window->setTitle("Feature Showcase > BasicEffect Test");
        subGame = std::make_shared<feature_showcase::BasicEffectTest>(gameHost);
    });
    buttons.emplace_back("GLTFModel Test", [this] {
        window->setTitle("Feature Showcase > GLTFModel Test");
        subGame = std::make_shared<feature_showcase::GLTFModelTest>(gameHost);
    });
    buttons.emplace_back("HardwareInstancing Test", [this] {
        window->setTitle("Feature Showcase > HardwareInstancing Test");
        subGame = std::make_shared<feature_showcase::HardwareInstancingTest>(gameHost);
    });
    buttons.emplace_back("MultiRenderTarget Test", [this] {
        window->setTitle("Feature Showcase > MultiRenderTarget Test");
        subGame = std::make_shared<feature_showcase::MultiRenderTargetTest>(gameHost);
    });
    buttons.emplace_back("SpriteBatch Test", [this] {
        window->setTitle("Feature Showcase > SpriteBatch Test");
        subGame = std::make_shared<feature_showcase::SpriteBatchTest>(gameHost);
    });
    buttons.emplace_back("SpriteFont Test", [this] {
        window->setTitle("Feature Showcase > SpriteFont Test");
        subGame = std::make_shared<feature_showcase::SpriteFontTest>(gameHost);
    });
    buttons.emplace_back("DistanceFieldFont Test", [this] {
        window->setTitle("Feature Showcase > DistanceFieldFont Test");
        subGame = std::make_shared<feature_showcase::DistanceFieldFontTest>(gameHost);
    });
    buttons.emplace_back("SpriteLine Test", [this] {
        window->setTitle("Feature Showcase > SpriteLine Test");
        subGame = std::make_shared<feature_showcase::SpriteLineTest>(gameHost);
    });
    buttons.emplace_back("BillboardBatch Test", [this] {
        window->setTitle("Feature Showcase > BillboardBatch Test");
        subGame = std::make_shared<feature_showcase::BillboardBatchTest>(gameHost);
    });
    buttons.emplace_back("ImageEffects Test", [this] {
        window->setTitle("Feature Showcase > ImageEffects Test");
        subGame = std::make_shared<feature_showcase::ImageEffectsTest>(gameHost);
    });
    buttons.emplace_back("Particle2D Test", [this] {
        window->setTitle("Feature Showcase > Particle2D Test");
        subGame = std::make_shared<feature_showcase::Particle2DTest>(gameHost);
    });
    buttons.emplace_back("ParticleClipLoader Test", [this] {
        window->setTitle("Feature Showcase > ParticleClipLoader Test");
        subGame = std::make_shared<feature_showcase::ParticleClipLoaderTest>(gameHost);
    });
    buttons.emplace_back("Particle3D Test", [this] {
        window->setTitle("Feature Showcase > Particle3D Test");
        subGame = std::make_shared<feature_showcase::Particle3DTest>(gameHost);
    });
    buttons.emplace_back("Beam2D Test", [this] {
        window->setTitle("Feature Showcase > Beam2D Test");
        subGame = std::make_shared<feature_showcase::Beam2DTest>(gameHost);
    });
    buttons.emplace_back("HTTPClient Test", [this] {
        window->setTitle("Feature Showcase > HTTPClient Test");
        subGame = std::make_shared<feature_showcase::HTTPClientTest>(gameHost);
    });
    buttons.emplace_back("AudioClip Test", [this] {
        window->setTitle("Feature Showcase > AudioClip Test");
        subGame = std::make_shared<feature_showcase::AudioClipTest>(gameHost);
    });
    buttons.emplace_back("Texture2DLoader Test", [this] {
        window->setTitle("Feature Showcase > Texture2DLoader Test");
        subGame = std::make_shared<feature_showcase::Texture2DLoaderTest>(gameHost);
    });
    buttons.emplace_back("SVGDecode Test", [this] {
        window->setTitle("Feature Showcase > SVGDecode Test");
        subGame = std::make_shared<feature_showcase::SVGDecodeTest>(gameHost);
    });
    buttons.emplace_back("VoxelModel Test", [this] {
        window->setTitle("Feature Showcase > VoxelModel Test");
        subGame = std::make_shared<feature_showcase::VoxelModelTest>(gameHost);
    });
    buttons.emplace_back("Gamepad Test", [this] {
        window->setTitle("Feature Showcase > Gamepad Test");
        subGame = std::make_shared<feature_showcase::GamepadTest>(gameHost);
    });
    buttons.emplace_back("Skeletal2D Test", [this] {
        window->setTitle("Feature Showcase > Skeletal2D Test");
        subGame = std::make_shared<feature_showcase::Skeletal2DTest>(gameHost);
    });
    buttons.emplace_back("Skinning2D Test", [this] {
        window->setTitle("Feature Showcase > Skinning2D Test");
        subGame = std::make_shared<feature_showcase::Skinning2DTest>(gameHost);
    });
    buttons.emplace_back("AnimationGraph Test", [this] {
        window->setTitle("Feature Showcase > AnimationGraph Test");
        subGame = std::make_shared<feature_showcase::AnimationGraphTest>(gameHost);
    });

    hudButtons.emplace_back("Back", [this] {
        window->setTitle("Feature Showcase");
        subGame.reset();
    });

    auto mouse = gameHost->getMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        if (subGame) {
            for (auto& button : hudButtons) {
                if (button.Selected) {
                    button.OnClicked();
                }
            }
        }
        else {
            for (auto& button : buttons) {
                if (button.Selected) {
                    button.OnClicked();
                    if (subGame) {
                        if (auto err = subGame->initialize(); err != nullptr) {
                            Log::Critical("Error", "failed to initialize game: " + err->toString());
                            subGame.reset();
                        }
                    }
                }
            }
        }
    });

    fpsTimer = std::make_shared<Timer>(clock);
    fpsTimer->SetInterval(std::chrono::milliseconds(150));
    connect(fpsTimer->Elapsed, [this] {
        footerString = StringHelper::Format("%.2f fps", clock->GetFrameRate());
    });

    return nullptr;
}

void GameMain::update()
{
    if (subGame) {
        subGame->update();
    }

    updateMenuLayout();

    const auto mouse = gameHost->getMouse();
    const auto mouseState = mouse->GetState();
    const auto clientBounds = window->getClientBounds();
    auto position = mouseState.Position;
    position.y = clientBounds.height - position.y;

    if (subGame) {
        for (auto& button : hudButtons) {
            button.Selected = button.Rect.contains(position);
        }
    }
    else {
        for (auto& button : buttons) {
            button.Selected = button.Rect.contains(position);
        }
    }

    connect(mouse->ScrollWheel, [this](std::int32_t delta) {
#if defined(POMDOG_PLATFORM_WIN32)
        // FIXME: Set to appropriate wheel scroll speed for each platform.
        constexpr double divisor = 0.001;
#else
        // NOTE: The answer to life, universe and everything.
        constexpr double divisor = 0.02;
#endif
        scrollY = std::clamp(scrollY + static_cast<double>(delta) * divisor, -540.0, 0.0);
    });
}

void GameMain::updateMenuLayout()
{
    const auto mouse = gameHost->getMouse();
    const auto clientBounds = window->getClientBounds();

    {
        int y = clientBounds.height - 36 - static_cast<int>(scrollY);
        constexpr int buttonVerticalMargin = 5;

        for (auto& button : buttons) {
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

        for (auto& button : hudButtons) {
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
    commandQueue->reset();

    if (subGame) {
        subGame->draw();
    }

    drawMenu();
    commandQueue->pushBackCommandList(commandList);

    commandQueue->executeCommandLists();
    commandQueue->present();
}

void GameMain::drawMenu()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

    std::optional<Vector4> clearColor;
    if (!subGame) {
        clearColor = Color{60, 60, 60, 255}.toVector4();
    }

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, clearColor};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<float>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    auto viewProjection = viewMatrix * projectionMatrix;

    auto drawButton = [this](const Button& button) {
        {
            Color color = Color{121, 120, 118, 255};
            if (button.Selected) {
                color = math::lerp(
                    Color{160, 160, 160, 255},
                    Color{191, 190, 180, 255},
                    0.5f + 0.5f * std::cos(math::TwoPi<float> * static_cast<float>(timer->GetTotalTime().count())));
            }

            primitiveBatch->DrawRectangle(
                Vector2{static_cast<float>(button.Rect.x), static_cast<float>(button.Rect.y)},
                static_cast<float>(button.Rect.width),
                static_cast<float>(button.Rect.height),
                Vector2{0.0f, 0.0f}, color);
        }
        {
            Vector2 position = {static_cast<float>(button.Rect.x), static_cast<float>(button.Rect.y)};

            Color color = Color{221, 220, 218, 255};
            if (button.Selected) {
                color = Color::createWhite();
            }

            position.x += 10.0f;
            position.y += 9.0f;

            spriteFont->Draw(
                *spriteBatch,
                button.Text,
                position,
                color,
                0.0f,
                Vector2{0.0f, 0.0f},
                1.0f);
        }
    };

    commandList->reset();
    commandList->setRenderPass(std::move(pass));
    primitiveBatch->Begin(commandList, viewProjection);
    spriteBatch->Begin(commandList, viewProjection);
    if (subGame) {
        for (const auto& button : hudButtons) {
            drawButton(button);
        }

        spriteFont->Draw(*spriteBatch,
            footerString,
            Vector2{static_cast<float>(viewport.width) - 8.0f, 8.0f},
            Color::createWhite(),
            0.0f,
            Vector2{1.0f, 0.0f},
            1.0f);
    }
    else {
        for (const auto& button : buttons) {
            drawButton(button);
        }
    }
    primitiveBatch->End();
    spriteBatch->End();

    commandList->close();
}

} // namespace feature_showcase
