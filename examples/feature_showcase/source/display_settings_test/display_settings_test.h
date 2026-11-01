// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"
#include "pomdog/gpu/graphics_device_info.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// InfoRow is a row of an information section, shown as a label and a value
/// and copied as "label: value".
struct InfoRow final {
    std::string_view label;
    std::string value;
};

/// CopyState tracks the Copy button of an information section.
struct CopyState final {
    /// The pending write request, or zero when none is pending.
    u64 requestID = 0;

    /// The result of the last copy, shown next to the button.
    std::string_view result;
};

class DisplaySettingsTest final : public Game {
public:
    explicit DisplaySettingsTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    void buildUI(devgui::UI& ui);

    void buildInfoSection(devgui::UI& ui, devgui::Key copyKey, std::string_view title, std::span<const InfoRow> rows, CopyState& copy);

    void applyWindowMode(WindowMode mode) noexcept;

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<GameWindow> window_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;

    DevGUIOverlay overlay_;
    std::shared_ptr<clipboard::ClipboardService> clipboard_;

    gpu::GraphicsDeviceInfo deviceInfo_;
    gpu::GraphicsBackend backendKind_ = {};
    CopyState displayMetricsCopy_;
    CopyState graphicsDeviceCopy_;

    WindowMode windowMode_ = WindowMode::Windowed;
    std::string windowModeError_;
    DisplayMetrics displayMetrics_;
    u32 displayMetricsChangeCount_ = 0;
    i32 frameRateCapIndex_ = 0;
    bool allowUserResizing_ = false;
    bool vsync_ = true;
};

} // namespace feature_showcase
