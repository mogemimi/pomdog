// Copyright mogemimi. Distributed under the MIT license.

#include "display_settings_test.h"
#include "pomdog/experimental/clipboard/platform_clipboard.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

constexpr std::array<std::string_view, 12> frameRateCapNames = {{
    "No Cap",
    "15",
    "24",
    "30",
    "45",
    "60",
    "90",
    "120",
    "144",
    "160",
    "180",
    "320",
}};

constexpr std::array<std::optional<i32>, 12> frameRateCaps = {{
    std::nullopt,
    15,
    24,
    30,
    45,
    60,
    90,
    120,
    144,
    160,
    180,
    320,
}};

[[nodiscard]] std::string_view
toString(WindowMode mode) noexcept
{
    switch (mode) {
    case WindowMode::Windowed:
        return "Windowed";
    case WindowMode::BorderlessWindowed:
        return "BorderlessWindowed";
    case WindowMode::Maximized:
        return "Maximized";
    case WindowMode::Fullscreen:
        return "Fullscreen";
    case WindowMode::BrowserSoftFullscreen:
        return "BrowserSoftFullscreen";
    }
    return "Unknown";
}

[[nodiscard]] std::string_view
toString(gpu::GraphicsBackend backend) noexcept
{
    switch (backend) {
    case gpu::GraphicsBackend::Direct3D11:
        return "Direct3D 11";
    case gpu::GraphicsBackend::OpenGL4:
        return "OpenGL 4";
    case gpu::GraphicsBackend::Metal:
        return "Metal";
    case gpu::GraphicsBackend::WebGL:
        return "WebGL";
    case gpu::GraphicsBackend::Vulkan:
        return "Vulkan";
    }
    return "Unknown";
}

[[nodiscard]] std::string_view
toCopyResult(clipboard::ClipboardStatus status) noexcept
{
    switch (status) {
    case clipboard::ClipboardStatus::Succeeded:
        return "Copied";
    case clipboard::ClipboardStatus::NoText:
        return "NoText";
    case clipboard::ClipboardStatus::Unsupported:
        return "Unsupported";
    case clipboard::ClipboardStatus::Denied:
        return "Denied";
    case clipboard::ClipboardStatus::Failed:
        return "Failed";
    }
    return "Unknown";
}

/// Joins rows into lines of "label: value".
[[nodiscard]] std::string
joinInfoRows(std::span<const InfoRow> rows)
{
    std::string text;
    for (const auto& row : rows) {
        if (!text.empty()) {
            text += '\n';
        }
        text += pomdog::format("{}: {}", row.label, row.value);
    }
    return text;
}

} // namespace

DisplaySettingsTest::DisplaySettingsTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
DisplaySettingsTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    window_ = gameHost_->getWindow();

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

    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    // NOTE: The Copy buttons write the rows of a section to the platform's clipboard.
    if (auto [p, err] = clipboard::createClipboardService(gameHost_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create clipboard service");
    }
    else {
        clipboard_ = std::move(p);
    }

    deviceInfo_ = graphicsDevice_->getDeviceInfo();
    backendKind_ = graphicsDevice_->getBackendKind();

    // NOTE: The panel shows the mode that the last windowModeChanged signal reported.
    connect_(window_->windowModeChanged, [this](WindowMode mode) {
        windowMode_ = mode;
    });
    windowMode_ = window_->getWindowMode();

    // NOTE: The panel shows the metrics that the last displayMetricsChanged
    // signal reported, and how many signals arrived.
    connect_(window_->displayMetricsChanged, [this](const DisplayMetrics& metrics) {
        displayMetrics_ = metrics;
        ++displayMetricsChangeCount_;
    });
    displayMetrics_ = window_->getDisplayMetrics();

    allowUserResizing_ = window_->getAllowUserResizing();
    vsync_ = (gameHost_->getPresentMode() != gpu::PresentMode::Immediate);
    return nullptr;
}

void DisplaySettingsTest::applyWindowMode(WindowMode mode) noexcept
{
    windowModeError_.clear();
    if (auto err = window_->requestWindowMode(mode); err != nullptr) {
        windowModeError_ = err->toString();
    }
}

void DisplaySettingsTest::buildUI(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{300.0f, 420.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "displaySettings"_key, "Display Settings", options);
    if (!window.isOpen()) {
        return;
    }

    devgui::labelText(ui, "Frame Rate", pomdog::format("{:.2f} fps", gameHost_->getClock()->getFrameRate()));
    devgui::separator(ui);

    devgui::text(ui, "Display Settings", ui.getStyle().textSecondaryColor);
    if (devgui::checkbox(ui, "vsync"_key, "V-Sync", vsync_).changed) {
        gameHost_->requestPresentMode(vsync_ ? gpu::PresentMode::VSync : gpu::PresentMode::Immediate);
    }
    if (devgui::combo(ui, "frameRateCap"_key, "FPS Cap", frameRateCapIndex_, frameRateCapNames).changed) {
        gameHost_->setMaxFramesPerSecond(frameRateCaps[static_cast<std::size_t>(frameRateCapIndex_)]);
    }
    devgui::separator(ui);

    devgui::text(ui, "Window Mode", ui.getStyle().textSecondaryColor);
    devgui::labelText(ui, "Current", toString(windowMode_));
    if (!windowModeError_.empty()) {
        devgui::text(ui, windowModeError_, ui.getStyle().errorColor);
    }
    if (devgui::button(ui, "windowed"_key, "Windowed").clicked) {
        applyWindowMode(WindowMode::Windowed);
    }
    if (devgui::button(ui, "borderless"_key, "Borderless Windowed").clicked) {
        applyWindowMode(WindowMode::BorderlessWindowed);
    }
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    // NOTE: Maximizing is desktop only.
    if (devgui::button(ui, "maximized"_key, "Maximized").clicked) {
        applyWindowMode(WindowMode::Maximized);
    }
#endif
    if (devgui::button(ui, "fullscreen"_key, "Fullscreen").clicked) {
        applyWindowMode(WindowMode::Fullscreen);
    }
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    // NOTE: Soft fullscreen fills the browser window, on the web only.
    if (devgui::button(ui, "softFullscreen"_key, "Browser Soft Fullscreen").clicked) {
        applyWindowMode(WindowMode::BrowserSoftFullscreen);
    }
#endif
    devgui::separator(ui);

    devgui::text(ui, "Window Properties", ui.getStyle().textSecondaryColor);
    if (devgui::checkbox(ui, "allowResizing"_key, "Allow Resizing", allowUserResizing_).changed) {
        window_->requestAllowUserResizing(allowUserResizing_);
    }
    if (devgui::button(ui, "clientBounds"_key, "Client Bounds 800x600").clicked) {
        [[maybe_unused]] auto err = window_->requestClientBounds(Rect2D{0, 0, 800, 600});
    }

    // NOTE: The title contains Japanese (UTF-8) to check its encoding.
    if (devgui::button(ui, "title"_key, "Set Title (Japanese)").clicked) {
        window_->requestTitle("DisplaySettings \xe3\x83\x86\xe3\x82\xb9\xe3\x83\x88");
    }
    devgui::separator(ui);

    const std::array<InfoRow, 4> displayMetricsRows = {{
        {"Signals", pomdog::format("{}", displayMetricsChangeCount_)},
        {"Client", pomdog::format("{}x{}", displayMetrics_.clientBounds.width, displayMetrics_.clientBounds.height)},
        {"Back Buffer", pomdog::format("{}x{}", displayMetrics_.backBufferWidth, displayMetrics_.backBufferHeight)},
        {"Pixel Ratio", pomdog::format("{:.2f}", static_cast<f64>(displayMetrics_.pixelRatio))},
    }};
    buildInfoSection(ui, "copyDisplayMetrics"_key, "Display Metrics", displayMetricsRows, displayMetricsCopy_);
    devgui::separator(ui);

    const std::array<InfoRow, 3> graphicsDeviceRows = {{
        {"GPU", deviceInfo_.deviceName},
        {"Graphics API", std::string{toString(backendKind_)}},
        {"API Version", deviceInfo_.apiVersion},
    }};
    buildInfoSection(ui, "copyGraphicsDevice"_key, "Graphics Device", graphicsDeviceRows, graphicsDeviceCopy_);
}

void DisplaySettingsTest::buildInfoSection(devgui::UI& ui, devgui::Key copyKey, std::string_view title, std::span<const InfoRow> rows, CopyState& copy)
{
    {
        auto row = devgui::horizontal(ui);
        devgui::text(ui, title, ui.getStyle().textSecondaryColor);
        if (devgui::button(ui, copyKey, "Copy").clicked) {
            copy.result = {};
            copy.requestID = clipboard_->requestWriteText(joinInfoRows(rows));
        }
        if (!copy.result.empty()) {
            devgui::text(ui, copy.result, ui.getStyle().textSecondaryColor);
        }
    }
    for (const auto& row : rows) {
        devgui::labelText(ui, row.label, row.value);
    }
}

void DisplaySettingsTest::update()
{
    // NOTE: The result of a copy arrives as an event after a later update().
    clipboard_->update();
    for (const auto& event : clipboard_->getEvents()) {
        if (event.kind != clipboard::ClipboardEventKind::WriteCompleted) {
            continue;
        }
        for (auto copy : std::array{&displayMetricsCopy_, &graphicsDeviceCopy_}) {
            if (copy->requestID == event.requestID) {
                copy->requestID = 0;
                copy->result = toCopyResult(event.status);
            }
        }
    }

    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });
}

void DisplaySettingsTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    // NOTE: The grid is positioned in logical pixels, so the projection and
    // view matrix use the logical client size. The viewport below stays in
    // physical pixels.
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

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
        0.0f,
        100.0f);

    // NOTE: Draw background grid
    const auto w = static_cast<f32>(clientBounds.width);
    const auto h = static_cast<f32>(clientBounds.height);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

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
