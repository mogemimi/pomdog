#pragma once

#include "pomdog/experimental/devgui/devgui.h"
#include "pomdog/experimental/devgui/renderer.h"
#include "pomdog/graphics/sprite_font.h"
#include "pomdog/graphics/truetype_font.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class DevGUITest final : public Game {
public:
    explicit DevGUITest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    struct Emitter final {
        u32 id = 0;
        std::string name;
        i32 shapeIndex = 0;
        Color startColor = Color{255, 196, 64, 255};
        f32 emissionRate = 16.0f;
        f32 startSpeed = 120.0f;
        f32 startLifetime = 1.0f;
        f32 gravityModifier = 0.0f;
        i32 maxParticles = 128;
        std::vector<devgui::CurvePoint> sizeOverLifetime;
        bool looping = true;
    };

    /// Selects a change to the window layout that waits until the frame ends.
    enum class LayoutRequest : u8 {
        None,
        Restore,
        Reset,
    };

    void buildUI(devgui::UI& ui);

    /// Builds the menu bar along the top of the screen with its View menu.
    void buildMenuBar(devgui::UI& ui);

    void buildEmitterContextMenu(devgui::UI& ui, const devgui::Response& response, Emitter& emitter);

    void buildEmitterWindow(devgui::UI& ui);

    void buildWindowList(devgui::UI& ui);

    void buildPreviewWindow(devgui::UI& ui);

    /// Draws a still picture of where the particles of emitter go, around
    /// center within radius.
    void drawEmitterPreview(devgui::UI& ui, const Emitter& emitter, const Vector2& center, f32 radius) const;

    [[nodiscard]] Color
    getBackgroundColor() const noexcept;

    void buildLanguageWindow(devgui::UI& ui);

    void buildStatsWindow(devgui::UI& ui);

    void addEmitter();

    void buildDefaultLayout();

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;

    std::shared_ptr<devgui::Renderer> renderer_;
    std::unique_ptr<devgui::Context> context_;

    std::vector<Emitter> emitters_;
    std::string message_;
    u32 nextEmitterID_ = 1;
    std::optional<u32> removeRequest_;
    std::optional<u32> duplicateRequest_;
    std::string savedLayout_;
    std::string moduleQuery_;
    std::vector<std::string_view> moduleMatches_;
    std::string_view addedModule_;
    std::vector<devgui::DockedWindow> dockedWindows_;
    LayoutRequest layoutRequest_ = LayoutRequest::None;
    u64 skippedFrames_ = 0;
    i32 languageIndex_ = 0;
    i32 backgroundRed_ = 40;
    i32 backgroundGreen_ = 44;
    i32 backgroundBlue_ = 52;
    bool showStats_ = true;
    bool showLanguages_ = true;
    bool showTabBars_ = true;

    /// The UI scale on top of the window's pixel ratio, chosen in the View menu.
    f32 uiScale_ = 1.0f;
    bool antialiasing_ = true;
};

} // namespace feature_showcase
