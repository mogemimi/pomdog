#include "audio_clip_test.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

struct AudioFile final {
    std::string_view label;
    std::string_view path;
    bool streaming = false;
};

constexpr std::array<AudioFile, 7> audioFiles = {{
    {"Load: pong1.wav", "/assets/sounds/pong1.wav", false},
    {"Load: synth.ogg", "/assets/sounds/synth.ogg", false},
    {"Load: ambient_lakeside_oneshot.wav", "/assets/sounds/ambient_lakeside_oneshot.wav", false},
    {"Stream: pong1.wav", "/assets/sounds/pong1.wav", true},
    {"Stream: synth.ogg", "/assets/sounds/synth.ogg", true},
    {"Stream: ambient_lakeside_wav.wav", "/assets/sounds/ambient_lakeside_wav.wav", true},
    {"Stream: ambient_lakeside_vorbis.ogg", "/assets/sounds/ambient_lakeside_vorbis.ogg", true},
}};

} // namespace

AudioClipTest::AudioClipTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
AudioClipTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    audioEngine_ = gameHost_->getAudioEngine();
    audioEngine_->setMainVolume(mainVolume_);

    return nullptr;
}

void AudioClipTest::loadAudio(std::string_view filePath, bool streaming)
{
    audioSource_ = nullptr;
    audioClip_ = nullptr;
    isStreaming_ = streaming;
    errorText_.clear();

    auto [clip, clipErr] = streaming
                               ? loadAudioClipStreaming(fs_, audioEngine_, filePath)
                               : loadAudioClip(fs_, audioEngine_, filePath);
    if (clipErr != nullptr) {
        errorText_ = clipErr->toString();
        return;
    }
    audioClip_ = std::move(clip);

    if (auto [source, err] = audioEngine_->createAudioSource(audioClip_, isLooped_); err != nullptr) {
        errorText_ = err->toString();
        return;
    }
    else {
        audioSource_ = std::move(source);
    }
    audioSource_->setVolume(audioVolume_);
}

void AudioClipTest::buildUI(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{320.0f, 420.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "audio"_key, "Audio Clip", options);
    if (!window.isOpen()) {
        return;
    }

    for (u32 i = 0; i < audioFiles.size(); ++i) {
        const auto& file = audioFiles[i];
        if (devgui::button(ui, "file"_key.at(i), file.label).clicked) {
            loadAudio(file.path, file.streaming);
        }
    }
    devgui::separator(ui);

    // NOTE: Looping applies to the source that the next load creates.
    devgui::checkbox(ui, "looped"_key, "Looped", isLooped_);
    if (devgui::sliderFloat(ui, "mainVolume"_key, "Main Volume", mainVolume_, 0.0f, 1.0f).changed) {
        audioEngine_->setMainVolume(mainVolume_);
    }
    if (devgui::sliderFloat(ui, "audioVolume"_key, "Audio Volume", audioVolume_, 0.0f, 1.0f).changed && (audioSource_ != nullptr)) {
        audioSource_->setVolume(audioVolume_);
    }
    {
        auto row = devgui::horizontal(ui);
        if (devgui::button(ui, "play"_key, "Play").clicked && (audioSource_ != nullptr)) {
            audioSource_->play();
        }
        if (devgui::button(ui, "pause"_key, "Pause").clicked && (audioSource_ != nullptr)) {
            audioSource_->pause();
        }
        if (devgui::button(ui, "stop"_key, "Stop").clicked && (audioSource_ != nullptr)) {
            audioSource_->stop();
        }
    }
    devgui::separator(ui);

    if (!errorText_.empty()) {
        devgui::text(ui, errorText_);
        return;
    }
    if (audioClip_ == nullptr) {
        devgui::text(ui, "No audio loaded");
        return;
    }
    devgui::labelText(ui, "Samples", pomdog::format("{}", audioClip_->getSampleCount()));
    devgui::labelText(ui, "Bits", pomdog::format("{}", audioClip_->getBitsPerSample()));
    devgui::labelText(ui, "Channels", (audioClip_->getChannels() == AudioChannels::Stereo) ? "Stereo" : "Mono");
    devgui::labelText(ui, "Rate", pomdog::format("{}", audioClip_->getSampleRate()));
    devgui::labelText(ui, "Length", pomdog::format("{:.3f}s", audioClip_->getLength().count()));
    devgui::labelText(ui, "Streaming", isStreaming_ ? "Yes" : "No");
}

void AudioClipTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });
}

void AudioClipTest::draw()
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
