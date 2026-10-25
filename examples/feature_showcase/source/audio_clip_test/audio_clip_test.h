#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class AudioClipTest final : public Game {
public:
    explicit AudioClipTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    void buildUI(devgui::UI& ui);

    void loadAudio(std::string_view filePath, bool streaming);

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;

    DevGUIOverlay overlay_;

    std::shared_ptr<AudioEngine> audioEngine_;
    std::shared_ptr<AudioClip> audioClip_;
    std::shared_ptr<AudioSource> audioSource_;

    std::string errorText_;
    f32 mainVolume_ = 0.3f;
    f32 audioVolume_ = 1.0f;
    bool isLooped_ = false;
    bool isStreaming_ = false;
};

} // namespace feature_showcase
