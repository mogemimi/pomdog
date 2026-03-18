#include "game_main.h"
#include "animation_graph_test/animation_graph_test.h"
#include "audio_clip_test/audio_clip_test.h"
#include "basic_effect_test/basic_effect_test.h"
#include "beam2d_test/beam2d_test.h"
#include "billboard_batch_test/billboard_batch_test.h"
#include "bug_issue_49_test/bug_issue_49_test.h"
#include "distance_field_font_test/distance_field_font_test.h"
#include "editor_gui_test/editor_gui_test.h"
#include "gamepad_test/gamepad_test.h"
#include "gltf_model_test/gltf_model_test.h"
#include "gui_splitter_test/gui_splitter_test.h"
#include "hardware_instancing_test/hardware_instancing_test.h"
#include "http_client_test/http_client_test.h"
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
#include "sprite_batch_test/sprite_batch_test.h"
#include "sprite_font_test/sprite_font_test.h"
#include "sprite_line_test/sprite_line_test.h"
#include "svg_decode_test/svg_decode_test.h"
#include "texture2d_loader_test/texture2d_loader_test.h"
#include "voxel_model_test/voxel_model_test.h"
#include "pomdog/utility/cli_parser.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file_archive.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

GameMain::GameMain() = default;

std::unique_ptr<Error>
GameMain::initialize(const std::shared_ptr<GameHost>& gameHostIn, int argc, const char* const* argv)
{
    gameHost_ = gameHostIn;
    window_ = gameHostIn->getWindow();
    graphicsDevice_ = gameHostIn->getGraphicsDevice();
    clock_ = gameHostIn->getClock();
    commandQueue_ = gameHostIn->getCommandQueue();

    // NOTE: Parse command-line arguments for VFS configuration
    std::string assetsDir;
    std::string archiveFile;
    {
        CLIParser cli;
        cli.add(&assetsDir, "assets-dir", "path to the assets directory");
        cli.add(&archiveFile, "archive-file", "path to the archive file (without extension)");
        if (auto err = cli.parse(argc, argv); err != nullptr) {
            return errors::wrap(std::move(err), "failed to parse command-line arguments");
        }
    }

    if (archiveFile.empty()) {
        auto [resourceDir, resourceDirErr] = FileSystem::getResourceDirectoryPath();
        if (resourceDirErr != nullptr) {
            return errors::wrap(std::move(resourceDirErr), "failed to get resource directory path");
        }
        archiveFile = filepaths::join(resourceDir, "content.idx");
    }

    // NOTE: Initialize VFS
    {
        if (auto [ctx, err] = vfs::create(); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create VFS");
        }
        else {
            fs_ = std::move(ctx);
        }

        if (!archiveFile.empty()) {
            const auto replaceExtension = [](std::string_view filename, std::string_view newExtension) -> std::string {
                auto [base, ext] = filepaths::splitExtensionAsView(filename);
                auto baseStr = std::string(base);
                baseStr += newExtension;
                return baseStr;
            };

            auto [vol, volErr] = vfs::openArchiveFile(archiveFile, replaceExtension(archiveFile, ".pak"));
            if (volErr != nullptr) {
                return errors::wrap(std::move(volErr), "failed to open archive file");
            }
            if (auto mountErr = vfs::mount(fs_, "/assets", std::move(vol), {.readOnly = true, .hashKeyLookup = true}); mountErr != nullptr) {
                return errors::wrap(std::move(mountErr), "failed to mount archive");
            }
        }
        if (!assetsDir.empty()) {
            if (auto mountErr = vfs::mount(fs_, "/assets", assetsDir, {.readOnly = true, .overlayFS = true}); mountErr != nullptr) {
                return errors::wrap(std::move(mountErr), "failed to mount assets directory");
            }
        }
    }

    window_->setTitle("Feature Showcase");
    window_->setAllowUserResizing(true);

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

    spriteFont_ = std::make_shared<SpriteFont>();
    if (auto spriteFontErr = spriteFont_->initialize(graphicsDevice_, font, 20.0f, 20.0f); spriteFontErr != nullptr) {
        return errors::wrap(std::move(spriteFontErr), "failed to initialize SpriteFont");
    }
    spriteBatch_ = std::make_shared<SpriteBatch>();
    if (auto spriteBatchErr = spriteBatch_->initialize(fs_, graphicsDevice_); spriteBatchErr != nullptr) {
        return errors::wrap(std::move(spriteBatchErr), "failed to initialize SpriteBatch");
    }
    spriteFont_->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
    primitiveBatch_ = std::make_shared<PrimitiveBatch>();
    if (auto primitiveBatchErr = primitiveBatch_->initialize(fs_, graphicsDevice_); primitiveBatchErr != nullptr) {
        return errors::wrap(std::move(primitiveBatchErr), "failed to initialize PrimitiveBatch");
    }
    timer_ = std::make_shared<Timer>(clock_);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(1.0);

    buttons_.emplace_back("EditorGUI Test", [this] {
        window_->setTitle("Feature Showcase > EditorGUI Test");
        subGame_ = std::make_shared<feature_showcase::EditorGUITest>(gameHost_, fs_);
    });
    buttons_.emplace_back("GUISplitter Test", [this] {
        window_->setTitle("Feature Showcase > GUISplitter Test");
        subGame_ = std::make_shared<feature_showcase::GUISplitterTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("LineBatch Test", [this] {
        window_->setTitle("Feature Showcase > LineBatch Test");
        subGame_ = std::make_shared<feature_showcase::LineBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("PolylineDrawing Test", [this] {
        window_->setTitle("Feature Showcase > PolylineDrawing Test");
        subGame_ = std::make_shared<feature_showcase::PolylineDrawingTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("PrimitiveBatch Test", [this] {
        window_->setTitle("Feature Showcase > PrimitiveBatch Test");
        subGame_ = std::make_shared<feature_showcase::PrimitiveBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("BasicEffect Test", [this] {
        window_->setTitle("Feature Showcase > BasicEffect Test");
        subGame_ = std::make_shared<feature_showcase::BasicEffectTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("GLTFModel Test", [this] {
        window_->setTitle("Feature Showcase > GLTFModel Test");
        subGame_ = std::make_shared<feature_showcase::GLTFModelTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("HardwareInstancing Test", [this] {
        window_->setTitle("Feature Showcase > HardwareInstancing Test");
        subGame_ = std::make_shared<feature_showcase::HardwareInstancingTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("MultiRenderTarget Test", [this] {
        window_->setTitle("Feature Showcase > MultiRenderTarget Test");
        subGame_ = std::make_shared<feature_showcase::MultiRenderTargetTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteBatch Test", [this] {
        window_->setTitle("Feature Showcase > SpriteBatch Test");
        subGame_ = std::make_shared<feature_showcase::SpriteBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteFont Test", [this] {
        window_->setTitle("Feature Showcase > SpriteFont Test");
        subGame_ = std::make_shared<feature_showcase::SpriteFontTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("DistanceFieldFont Test", [this] {
        window_->setTitle("Feature Showcase > DistanceFieldFont Test");
        subGame_ = std::make_shared<feature_showcase::DistanceFieldFontTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SpriteLine Test", [this] {
        window_->setTitle("Feature Showcase > SpriteLine Test");
        subGame_ = std::make_shared<feature_showcase::SpriteLineTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("BillboardBatch Test", [this] {
        window_->setTitle("Feature Showcase > BillboardBatch Test");
        subGame_ = std::make_shared<feature_showcase::BillboardBatchTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("ImageEffects Test", [this] {
        window_->setTitle("Feature Showcase > ImageEffects Test");
        subGame_ = std::make_shared<feature_showcase::ImageEffectsTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Particle2D Test", [this] {
        window_->setTitle("Feature Showcase > Particle2D Test");
        subGame_ = std::make_shared<feature_showcase::Particle2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("ParticleClipLoader Test", [this] {
        window_->setTitle("Feature Showcase > ParticleClipLoader Test");
        subGame_ = std::make_shared<feature_showcase::ParticleClipLoaderTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Particle3D Test", [this] {
        window_->setTitle("Feature Showcase > Particle3D Test");
        subGame_ = std::make_shared<feature_showcase::Particle3DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Beam2D Test", [this] {
        window_->setTitle("Feature Showcase > Beam2D Test");
        subGame_ = std::make_shared<feature_showcase::Beam2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("HTTPClient Test", [this] {
        window_->setTitle("Feature Showcase > HTTPClient Test");
        subGame_ = std::make_shared<feature_showcase::HTTPClientTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("AudioClip Test", [this] {
        window_->setTitle("Feature Showcase > AudioClip Test");
        subGame_ = std::make_shared<feature_showcase::AudioClipTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Texture2DLoader Test", [this] {
        window_->setTitle("Feature Showcase > Texture2DLoader Test");
        subGame_ = std::make_shared<feature_showcase::Texture2DLoaderTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("SVGDecode Test", [this] {
        window_->setTitle("Feature Showcase > SVGDecode Test");
        subGame_ = std::make_shared<feature_showcase::SVGDecodeTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("VoxelModel Test", [this] {
        window_->setTitle("Feature Showcase > VoxelModel Test");
        subGame_ = std::make_shared<feature_showcase::VoxelModelTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Gamepad Test", [this] {
        window_->setTitle("Feature Showcase > Gamepad Test");
        subGame_ = std::make_shared<feature_showcase::GamepadTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Skeletal2D Test", [this] {
        window_->setTitle("Feature Showcase > Skeletal2D Test");
        subGame_ = std::make_shared<feature_showcase::Skeletal2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Skinning2D Test", [this] {
        window_->setTitle("Feature Showcase > Skinning2D Test");
        subGame_ = std::make_shared<feature_showcase::Skinning2DTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("AnimationGraph Test", [this] {
        window_->setTitle("Feature Showcase > AnimationGraph Test");
        subGame_ = std::make_shared<feature_showcase::AnimationGraphTest>(gameHost_, fs_);
    });
    buttons_.emplace_back("Bug Issue #49", [this] {
        window_->setTitle("Feature Showcase > Bug Issue #49 Test");
        subGame_ = std::make_shared<feature_showcase::BugIssue49Test>(gameHost_, fs_);
    });

    hudButtons_.emplace_back("Back", [this] {
        window_->setTitle("Feature Showcase");
        subGame_.reset();
    });

    auto mouse = gameHost_->getMouse();
    connect_(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

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
                        if (auto err = subGame_->initialize(gameHost_, 0, nullptr); err != nullptr) {
                            Log::Critical("Error", "failed to initialize game: " + err->toString());
                            subGame_.reset();
                        }
                    }
                }
            }
        }
    });

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
    const auto mouseState = mouse->getState();
    const auto clientBounds = window_->getClientBounds();
    auto position = mouseState.position;
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

    connect_(mouse->ScrollWheel, [this](std::int32_t delta) {
#if defined(POMDOG_PLATFORM_WIN32)
        // FIXME: Set to appropriate wheel scroll speed for each platform.
        constexpr double divisor = 0.001;
#else
        // NOTE: The answer to life, universe and everything.
        constexpr double divisor = 0.02;
#endif
        scrollY_ = std::clamp(scrollY_ + static_cast<double>(delta) * divisor, -540.0, 0.0);
    });
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
                    0.5f + 0.5f * std::cos(math::TwoPi<float> * static_cast<float>(timer_->getTotalTime().count())));
            }

            primitiveBatch_->drawRectangle(
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

            spriteFont_->draw(
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
    commandList_->setRenderPass(std::move(pass));
    primitiveBatch_->begin(commandList_, viewProjection);
    spriteBatch_->begin(commandList_, viewProjection);
    if (subGame_) {
        for (const auto& button : hudButtons_) {
            drawButton(button);
        }

        spriteFont_->draw(*spriteBatch_,
            footerString_,
            Vector2{static_cast<float>(viewport.width) - 8.0f, 8.0f},
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
    primitiveBatch_->end();
    spriteBatch_->end();

    commandList_->close();
}

} // namespace feature_showcase
