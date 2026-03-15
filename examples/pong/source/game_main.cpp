#include "game_main.h"
#include "pomdog/experimental/image_effects/chromatic_aberration.h"
#include "pomdog/experimental/image_effects/fish_eye_effect.h"
#include "pomdog/experimental/image_effects/fxaa.h"
#include "pomdog/experimental/image_effects/retro_crt_effect.h"
#include "pomdog/experimental/image_effects/vignette_effect.h"
#include "pomdog/utility/cli_parser.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file_archive.h"
#include <cmath>
#include <random>
#include <utility>

namespace pong {

GameMain::GameMain() = default;

std::unique_ptr<Error>
GameMain::initialize(const std::shared_ptr<GameHost>& gameHostIn, int argc, const char* const* argv)
{
    gameHost_ = gameHostIn;
    window_ = gameHostIn->getWindow();
    graphicsDevice_ = gameHostIn->getGraphicsDevice();
    commandQueue_ = gameHostIn->getCommandQueue();
    clock_ = gameHostIn->getClock();
    audioEngine_ = gameHostIn->getAudioEngine();
    postProcessCompositor_ = std::make_unique<PostProcessCompositor>();
    if (auto pcErr = postProcessCompositor_->initialize(gameHostIn->getGraphicsDevice()); pcErr != nullptr) {
        return errors::wrap(std::move(pcErr), "failed to initialize post process compositor");
    }
    textTimer_ = std::make_unique<Timer>(clock_);

    window_->setAllowUserResizing(true);

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

    // NOTE: Set window name
    window_->setTitle("Pomdog Pong");

    // NOTE: Set main volume
    audioEngine_->setMainVolume(0.4f);

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Create batch renderers
    primitiveBatch_ = std::make_shared<PrimitiveBatch>(graphicsDevice_);
    spriteBatch_ = std::make_shared<SpriteBatch>(graphicsDevice_);

    // NOTE: Prepare sprite font
    auto [font, fontErr] = loadTrueTypeFont(fs_, "/assets/fonts/NotoSans-BoldItalic.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }
    spriteFont_ = std::make_shared<SpriteFont>(graphicsDevice_, font, 26.0f, 26.0f);
    spriteFont_->prepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
    spriteFont_->setDefaultCharacter(U'?');

    // NOTE: Load sound effects
    if (auto [audioClip, loadErr] = loadAudioClip(fs_, audioEngine_, "/assets/sounds/pong1.wav"); loadErr != nullptr) {
        return errors::wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        if (auto [soundEffect, err] = audioEngine_->createSoundEffect(audioClip, false); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        else {
            soundEffect1_ = std::move(soundEffect);
        }
    }

    if (auto [audioClip, loadErr] = loadAudioClip(fs_, audioEngine_, "/assets/sounds/pong2.wav"); loadErr != nullptr) {
        return errors::wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        if (auto [soundEffect, err] = audioEngine_->createSoundEffect(audioClip, false); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        else {
            soundEffect2_ = std::move(soundEffect);
        }
    }

    if (auto [audioClip, loadErr] = loadAudioClip(fs_, audioEngine_, "/assets/sounds/pong3.wav"); loadErr != nullptr) {
        return errors::wrap(std::move(loadErr), "failed to load audio clip");
    }
    else {
        if (auto [soundEffect, err] = audioEngine_->createSoundEffect(audioClip, false); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create sound effect");
        }
        else {
            soundEffect3_ = std::move(soundEffect);
        }
    }

    {
        auto fxaa = std::make_shared<FXAA>();
        if (auto fxaaErr = fxaa->initialize(fs_, graphicsDevice_); fxaaErr != nullptr) {
            return errors::wrap(std::move(fxaaErr), "failed to initialize FXAA");
        }
        auto fishEyeEffect = std::make_shared<FishEyeEffect>();
        if (auto fishErr = fishEyeEffect->initialize(fs_, graphicsDevice_); fishErr != nullptr) {
            return errors::wrap(std::move(fishErr), "failed to initialize FishEyeEffect");
        }
        auto vignetteEffect = std::make_shared<VignetteEffect>();
        if (auto vigErr = vignetteEffect->initialize(fs_, graphicsDevice_); vigErr != nullptr) {
            return errors::wrap(std::move(vigErr), "failed to initialize VignetteEffect");
        }
        auto chromaticAberration = std::make_shared<ChromaticAberration>();
        if (auto caErr = chromaticAberration->initialize(fs_, graphicsDevice_); caErr != nullptr) {
            return errors::wrap(std::move(caErr), "failed to initialize ChromaticAberration");
        }
        auto retroCrtEffect = std::make_shared<RetroCrtEffect>();
        if (auto retroErr = retroCrtEffect->initialize(fs_, graphicsDevice_); retroErr != nullptr) {
            return errors::wrap(std::move(retroErr), "failed to initialize RetroCrtEffect");
        }

        vignetteEffect->setIntensity(1.0f);
        fishEyeEffect->setStrength(0.2f);

        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        // NOTE: Create render target
        if (auto [rt, rtErr] = graphicsDevice_->createRenderTarget2D(
                presentationParameters.backBufferWidth,
                presentationParameters.backBufferHeight,
                false,
                presentationParameters.backBufferFormat);
            rtErr != nullptr) {
            return errors::wrap(std::move(rtErr), "failed to create render target");
        }
        else {
            renderTarget_ = std::move(rt);
        }

        // NOTE: Create depth stencil buffer
        if (auto [ds, dsErr] = graphicsDevice_->createDepthStencilBuffer(
                presentationParameters.backBufferWidth,
                presentationParameters.backBufferHeight,
                presentationParameters.depthStencilFormat);
            dsErr != nullptr) {
            return errors::wrap(std::move(dsErr), "failed to create depth stencil buffer");
        }
        else {
            depthStencilBuffer_ = std::move(ds);
        }

        if (auto viewportErr = postProcessCompositor_->setViewportSize(
                *graphicsDevice_,
                presentationParameters.backBufferWidth,
                presentationParameters.backBufferHeight,
                presentationParameters.depthStencilFormat);
            viewportErr != nullptr) {
            return errors::wrap(std::move(viewportErr), "failed to set viewport size");
        }

        postProcessCompositor_->composite({
            fxaa,
            retroCrtEffect,
            chromaticAberration,
            fishEyeEffect,
            vignetteEffect,
            chromaticAberration,
        });

        connect_(window_->clientSizeChanged, [this](int width, int height) {
            auto presentationParameters = graphicsDevice_->getPresentationParameters();

            auto [rt, rtErr] = graphicsDevice_->createRenderTarget2D(
                width,
                height,
                false,
                presentationParameters.backBufferFormat);
            renderTarget_ = std::move(rt);

            auto [ds, dsErr] = graphicsDevice_->createDepthStencilBuffer(
                width,
                height,
                presentationParameters.depthStencilFormat);
            depthStencilBuffer_ = std::move(ds);

            // NOTE: Ignore errors in resize callback
            [[maybe_unused]] auto err = postProcessCompositor_->setViewportSize(
                *graphicsDevice_,
                width,
                height,
                presentationParameters.depthStencilFormat);
        });
    }
    {
        gameFieldSize_ = Rect2D{0, 0, 380, 280};
        gameFieldSize_.x = -gameFieldSize_.width / 2;
        gameFieldSize_.y = -gameFieldSize_.height / 2;

        player1_.SetScore(0);
        player2_.SetScore(0);

        auto keyboard = gameHost_->getKeyboard();
        input1_.Reset(Keys::UpArrow, Keys::DownArrow, keyboard);
        input2_.Reset(Keys::W, Keys::S, keyboard);

        connect_(input1_.Up, [this] {
            paddle1_.positionOld = paddle1_.position;
            auto position = paddle1_.position;
            position.y += paddle1_.speed * static_cast<pomdog::f32>(clock_->getFrameDuration().count());

            if (position.y > (gameFieldSize_.height / 2 - paddle1_.height / 2)) {
                position.y = (gameFieldSize_.height / 2 - paddle1_.height / 2);
            }
            paddle1_.position = position;
        });

        connect_(input1_.Down, [this] {
            paddle1_.positionOld = paddle1_.position;
            auto position = paddle1_.position;
            position.y -= paddle1_.speed * static_cast<pomdog::f32>(clock_->getFrameDuration().count());

            if (position.y < -(gameFieldSize_.height / 2 - paddle1_.height / 2)) {
                position.y = -(gameFieldSize_.height / 2 - paddle1_.height / 2);
            }
            paddle1_.position = position;
        });

        connect_(input2_.Up, [this] {
            paddle2_.positionOld = paddle2_.position;
            auto position = paddle2_.position;
            position.y += paddle2_.speed * static_cast<pomdog::f32>(clock_->getFrameDuration().count());

            if (position.y > (gameFieldSize_.height / 2 - paddle2_.height / 2)) {
                position.y = (gameFieldSize_.height / 2 - paddle2_.height / 2);
            }
            paddle2_.position = position;
        });

        connect_(input2_.Down, [this] {
            paddle2_.positionOld = paddle2_.position;
            auto position = paddle2_.position;
            position.y -= paddle2_.speed * static_cast<pomdog::f32>(clock_->getFrameDuration().count());

            if (position.y < -(gameFieldSize_.height / 2 - paddle2_.height / 2)) {
                position.y = -(gameFieldSize_.height / 2 - paddle2_.height / 2);
            }
            paddle2_.position = position;
        });
    }
    {
        // Header text
        textTimer_->setInterval(std::chrono::milliseconds(500));
        connect_(textTimer_->elapsed, [this] {
            headerText_ = pomdog::format(
                "{:.0f} sec\n{:.0f} fps   SCORE {} - {}",
                clock_->getTotalGameTime().count(),
                std::round(clock_->getFrameRate()),
                player1_.GetScore(),
                player2_.GetScore());
        });

        paddle1_.position = Vector2{gameFieldSize_.width / 2.0f, 0.0f};
        paddle2_.position = Vector2{-gameFieldSize_.width / 2.0f, 0.0f};

        pongScene_ = PongScenes::StartWaiting;
    }

    return nullptr;
}

void GameMain::update()
{
    switch (pongScene_) {
    case PongScenes::StartWaiting: {
        ball_.position = Vector2::createZero();
        ball_.positionOld = Vector2::createZero();
        ball_.velocity = Vector2::createZero();

        auto keyboard = gameHost_->getKeyboard();
        startButtonConn_ = keyboard->KeyDown.connect([this](Keys key) {
            if (key == Keys::Space) {
                pongScene_ = PongScenes::Prepare;
            }
        });
        scoreTextVisible_ = true;
        pongScene_ = PongScenes::Waiting;
        break;
    }
    case PongScenes::Waiting: {
        input1_.Emit();
        input2_.Emit();
        break;
    }
    case PongScenes::Prepare: {
        scoreTextVisible_ = false;
        startButtonConn_.disconnect();

        const float speed = 420.0f;
        std::mt19937 random(std::random_device{}());
        std::uniform_real_distribution<float> distribution(0.7f, 1.0f);
        static bool flipflop = false;
        flipflop = !flipflop;
        ball_.velocity = math::normalize(Vector2{(flipflop ? -1.0f : 1.0f), distribution(random)}) * speed;

        soundEffect2_->stop();
        soundEffect2_->play();

        pongScene_ = PongScenes::Playing;
        break;
    }
    case PongScenes::Playing: {
        input1_.Emit();
        input2_.Emit();

        ball_.positionOld = ball_.position;
        auto position = ball_.position + (ball_.velocity * static_cast<pomdog::f32>(clock_->getFrameDuration().count()));
        ball_.position = position;

        bool collision = false;

        if (position.x >= 0) {
            auto paddleCollider = paddle1_.GetCollider();
            if (paddleCollider.intersects(ball_.GetCollider()) && ball_.positionOld.x <= paddleCollider.min.x) {
                ball_.velocity.x *= -1;
                collision = true;
            }
        }
        else {
            auto paddleCollider = paddle2_.GetCollider();
            if (paddleCollider.intersects(ball_.GetCollider()) && ball_.positionOld.x >= paddleCollider.max.x) {
                ball_.velocity.x *= -1;
                collision = true;
            }
        }

        const auto halfWidth = gameFieldSize_.width / 2;
        const auto halfHeight = gameFieldSize_.height / 2;

        if (position.y >= halfHeight || position.y <= -halfHeight) {
            ball_.velocity.y *= -1;
            position.y = ball_.positionOld.y;
            ball_.position = position;
            collision = true;
        }

        const float offset = 70.0f;
        if (position.x >= (halfWidth + offset)) {
            player1_.SetScore(player1_.GetScore() + 1);
            soundEffect3_->stop();
            soundEffect3_->play();
            pongScene_ = PongScenes::StartWaiting;
        }
        else if (position.x <= -(halfWidth + offset)) {
            player2_.SetScore(player2_.GetScore() + 1);
            soundEffect3_->stop();
            soundEffect3_->play();
            pongScene_ = PongScenes::StartWaiting;
        }

        if (collision) {
            soundEffect1_->stop();
            soundEffect1_->play();
        }
        break;
    }
    default:
        break;
    }
}

void GameMain::draw()
{
    const auto backgroundColor = Color{32, 31, 30, 255};

    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {renderTarget_, backgroundColor.toVector4()};
    pass.depthStencilBuffer = depthStencilBuffer_;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    const auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadian(45.0f),
        viewport.getAspectRatio(),
        0.01f,
        1000.0f);

    const auto cameraPosition = Vector3{0, 0, -512.0f};
    const auto cameraRotation = math::toRadian(-15.0f);
    const auto viewMatrix = Matrix4x4::createRotationX(-cameraRotation) * Matrix4x4::createTranslation(-cameraPosition);
    const auto viewProjection = viewMatrix * projectionMatrix;

    commandList_->reset();
    commandList_->setRenderPass(std::move(pass));

    // NOTE: Draw primitives
    primitiveBatch_->begin(commandList_, viewProjection);
    {
        // NOTE: Draw background
        {
            // Rectangle outline
            auto p1 = Vector2(static_cast<float>(gameFieldSize_.getLeft()), static_cast<float>(gameFieldSize_.getBottom()));
            auto p2 = Vector2(static_cast<float>(gameFieldSize_.getLeft()), static_cast<float>(gameFieldSize_.getTop()));
            auto p3 = Vector2(static_cast<float>(gameFieldSize_.getRight()), static_cast<float>(gameFieldSize_.getTop()));
            auto p4 = Vector2(static_cast<float>(gameFieldSize_.getRight()), static_cast<float>(gameFieldSize_.getBottom()));
            primitiveBatch_->drawLine(p1, p2, Color::createWhite(), 2.0f);
            primitiveBatch_->drawLine(p2, p3, Color::createWhite(), 2.0f);
            primitiveBatch_->drawLine(p3, p4, Color::createWhite(), 2.0f);
            primitiveBatch_->drawLine(p4, p1, Color::createWhite(), 2.0f);
        }
        {
            // Dotted line
            int count = 32;
            float offset = 0.5f;
            float startY = -gameFieldSize_.height * 0.5f;
            float height = (gameFieldSize_.height + (gameFieldSize_.height / count * offset));
            for (int i = 0; i < count; ++i) {
                Vector2 start = {0.0f, height / count * i + startY};
                Vector2 end = {0.0f, height / count * (i + offset) + startY};
                primitiveBatch_->drawLine(start, end, Color::createWhite(), 2.0f);
            }
        }

        // Paddle 1
        primitiveBatch_->drawRectangle(paddle1_.position, 10.0f, paddle1_.height, {0.5f, 0.5f}, Color::createWhite());

        // Paddle 2
        primitiveBatch_->drawRectangle(paddle2_.position, 10.0f, paddle2_.height, {0.5f, 0.5f}, Color::createWhite());

        // Ball
        primitiveBatch_->drawCircle(ball_.position, 6.0f, 32, Color::createWhite());
    }
    primitiveBatch_->end();

    // NOTE: Draw sprites and fonts
    spriteBatch_->begin(commandList_, Matrix4x4::createScale(0.002f) * viewProjection);
    spriteFont_->draw(*spriteBatch_, "", Vector2::createZero(), Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    {
        // Header Text
        spriteFont_->draw(
            *spriteBatch_,
            headerText_,
            Vector2{-110, 140}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // Footer Text
        spriteFont_->draw(
            *spriteBatch_,
            "[SPACE] to start, WS and Up/Down to move",
            Vector2{-180, -170}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);
    }
    if (scoreTextVisible_) {
        // "Player 1" Text
        spriteFont_->draw(
            *spriteBatch_,
            "Player 1",
            Vector2{-135, 90}, Color::createYellow(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // "Player 2" Text
        spriteFont_->draw(
            *spriteBatch_,
            "Player 2",
            Vector2{65, 90}, Color::createYellow(), 0.0f, Vector2{0.0f, 0.0f}, 1.0f);

        // "Player 1" Score Text
        spriteFont_->draw(
            *spriteBatch_,
            std::to_string(player1_.GetScore()),
            Vector2{-110, 50}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 2.0f);

        // "Player 2" Score Text
        spriteFont_->draw(
            *spriteBatch_,
            std::to_string(player2_.GetScore()),
            Vector2{80, 50}, Color::createWhite(), 0.0f, Vector2{0.0f, 0.0f}, 2.0f);
    }
    spriteBatch_->end();

    postProcessCompositor_->draw(*commandList_, renderTarget_);

    commandList_->close();

    // NOTE: Execute graphics commands
    commandQueue_->reset();
    commandQueue_->pushBackCommandList(commandList_);
    commandQueue_->executeCommandLists();
    commandQueue_->present();
}

} // namespace pong
