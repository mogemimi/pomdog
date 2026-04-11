#include "audio_clip_test.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/utility/string_format.h"

namespace feature_showcase {

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
AudioClipTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Set up GUI
    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto drawingContextErr = drawingContext_->initialize(graphicsDevice_, fs_); drawingContextErr != nullptr) {
        return errors::wrap(std::move(drawingContextErr), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->getDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 300, 390);
        stackPanel->setPosition(Point2D{5, 40});
        stackPanel->setPadding(gui::Thickness{8, 2, 2, 8});
        hierarchy_->addChild(stackPanel);

        auto scrollView = std::make_shared<gui::ScrollView>(dispatcher, 280, 390);
        stackPanel->addChild(scrollView);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 230, 300);
        verticalLayout->setLayoutSpacing(6);
        scrollView->setWidget(verticalLayout);

        // NOTE: Load buttons
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Load: pong1.wav");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudio("/assets/sounds/pong1.wav"); });
            verticalLayout->addChild(button);
        }
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Load: synth.ogg");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudio("/assets/sounds/synth.ogg"); });
            verticalLayout->addChild(button);
        }
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Load: ambient_lakeside_oneshot.wav");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudio("/assets/sounds/ambient_lakeside_oneshot.wav"); });
            verticalLayout->addChild(button);
        }
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Stream: pong1.wav");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudioStreaming("/assets/sounds/pong1.wav"); });
            verticalLayout->addChild(button);
        }
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Stream: synth.ogg");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudioStreaming("/assets/sounds/synth.ogg"); });
            verticalLayout->addChild(button);
        }
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Stream: ambient_lakeside_wav.wav");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudioStreaming("/assets/sounds/ambient_lakeside_wav.wav"); });
            verticalLayout->addChild(button);
        }
        {
            auto button = std::make_shared<gui::PushButton>(dispatcher);
            button->setText("Stream: ambient_lakeside_vorbis.ogg");
            button->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(button->Click, [this]() { loadAudioStreaming("/assets/sounds/ambient_lakeside_vorbis.ogg"); });
            verticalLayout->addChild(button);
        }

        // NOTE: Looped toggle
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 250, 10);
            verticalLayout->addChild(row);

            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Looped");
            row->addChild(label);

            auto toggle = std::make_shared<gui::ToggleSwitch>(dispatcher);
            toggle->setOn(false);
            connect_(toggle->Toggled, [this](bool isOn) {
                isLooped_ = isOn;
            });
            row->addChild(toggle);
        }

        // NOTE: Volume slider
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 250, 10);
            verticalLayout->addChild(row);

            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Main Volume");
            row->addChild(label);

            auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 1.0);
            slider->setValue(0.3);
            row->addChild(slider);

            row->setStretchFactor(label, 1);
            row->setStretchFactor(slider, 2);

            connect_(slider->ValueChanged, [this](double value) {
                audioEngine_->setMainVolume(static_cast<float>(value));
            });
        }
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 250, 10);
            verticalLayout->addChild(row);

            auto label = std::make_shared<gui::TextBlock>(dispatcher);
            label->setColor(Color::createWhite());
            label->setText("Audio Volume");
            row->addChild(label);

            auto slider = std::make_shared<gui::Slider>(dispatcher, 0.0, 1.0);
            slider->setValue(1.0);
            row->addChild(slider);

            row->setStretchFactor(label, 1);
            row->setStretchFactor(slider, 2);

            connect_(slider->ValueChanged, [this](double value) {
                if (audioSource_ != nullptr) {
                    audioSource_->setVolume(static_cast<float>(value));
                }
            });
        }

        // NOTE: Play/Pause/Stop buttons
        {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 250, 10);
            verticalLayout->addChild(row);

            auto playButton = std::make_shared<gui::PushButton>(dispatcher);
            playButton->setText("Play");
            playButton->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(playButton->Click, [this]() {
                if (audioSource_ != nullptr) {
                    audioSource_->play();
                }
            });
            row->addChild(playButton);

            auto pauseButton = std::make_shared<gui::PushButton>(dispatcher);
            pauseButton->setText("Pause");
            pauseButton->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(pauseButton->Click, [this]() {
                if (audioSource_ != nullptr) {
                    audioSource_->pause();
                }
            });
            row->addChild(pauseButton);

            auto stopButton = std::make_shared<gui::PushButton>(dispatcher);
            stopButton->setText("Stop");
            stopButton->setHorizontalAlignment(gui::HorizontalAlignment::Stretch);
            connect_(stopButton->Click, [this]() {
                if (audioSource_ != nullptr) {
                    audioSource_->stop();
                }
            });
            row->addChild(stopButton);
        }

        // NOTE: Info text
        {
            infoText_ = std::make_shared<gui::TextBlock>(dispatcher);
            infoText_->setColor(Color{200, 200, 200, 255});
            infoText_->setText("No audio loaded");
            infoText_->setSize(250, 80);
            verticalLayout->addChild(infoText_);
        }
    }

    audioEngine_ = gameHost_->getAudioEngine();
    audioEngine_->setMainVolume(0.3f);

    return nullptr;
}

void AudioClipTest::loadAudio(std::string_view filePath)
{
    audioSource_ = nullptr;
    audioClip_ = nullptr;
    isStreaming_ = false;

    if (auto [clip, err] = loadAudioClip(fs_, audioEngine_, filePath); err != nullptr) {
        if (infoText_ != nullptr) {
            infoText_->setText(err->toString());
        }
        return;
    }
    else {
        audioClip_ = std::move(clip);
    }

    if (auto [source, err] = audioEngine_->createAudioSource(audioClip_, isLooped_); err != nullptr) {
        if (infoText_ != nullptr) {
            infoText_->setText(err->toString());
        }
        return;
    }
    else {
        audioSource_ = std::move(source);
    }

    if (infoText_ != nullptr && audioClip_ != nullptr) {
        auto info = pomdog::format(
            "samples: {}\n"
            "bits: {}\n"
            "channels: {}\n"
            "rate: {}\n"
            "length: {:.3f}s\n"
            "streaming: {}",
            audioClip_->getSampleCount(),
            audioClip_->getBitsPerSample(),
            (audioClip_->getChannels() == AudioChannels::Stereo) ? "Stereo" : "Mono",
            audioClip_->getSampleRate(),
            audioClip_->getLength().count(),
            isStreaming_ ? "yes" : "no");
        infoText_->setText(info);
    }
}

void AudioClipTest::loadAudioStreaming(std::string_view filePath)
{
    audioSource_ = nullptr;
    audioClip_ = nullptr;
    isStreaming_ = true;

    if (auto [clip, err] = loadAudioClipStreaming(fs_, audioEngine_, filePath); err != nullptr) {
        if (infoText_ != nullptr) {
            infoText_->setText(err->toString());
        }
        return;
    }
    else {
        audioClip_ = std::move(clip);
    }

    if (auto [source, err] = audioEngine_->createAudioSource(audioClip_, isLooped_); err != nullptr) {
        if (infoText_ != nullptr) {
            infoText_->setText(err->toString());
        }
        return;
    }
    else {
        audioSource_ = std::move(source);
    }

    if (infoText_ != nullptr && audioClip_ != nullptr) {
        auto info = pomdog::format(
            "samples: {}\n"
            "bits: {}\n"
            "channels: {}\n"
            "rate: {}\n"
            "length: {:.3f}s\n"
            "streaming: yes",
            audioClip_->getSampleCount(),
            audioClip_->getBitsPerSample(),
            (audioClip_->getChannels() == AudioChannels::Stereo) ? "Stereo" : "Mono",
            audioClip_->getSampleRate(),
            audioClip_->getLength().count());
        infoText_->setText(info);
    }
}

void AudioClipTest::update()
{
    hierarchy_->update();

    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->touch(mouse->getState());
    }

    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void AudioClipTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<float>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.backBufferHeight) * 0.5f,
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
