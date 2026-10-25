#include "http_client_test.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cctype>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

namespace {

constexpr auto requestNames = std::to_array<std::string_view>({
    "humans.txt (www.google.com)",
    "LICENSE.md (GitHub)",
    "Missing file (GitHub)",
    "Plain HTTP (example.com)",
});

constexpr auto requestURLs = std::to_array<std::string_view>({
    "https://www.google.com/humans.txt",
    "https://raw.githubusercontent.com/mogemimi/pomdog/main/LICENSE.md",
    "https://raw.githubusercontent.com/mogemimi/pomdog/main/not_found.txt",
    "http://example.com/",
});
static_assert(requestNames.size() == requestURLs.size());

/// Returns true if a and b are equal, ignoring the case of ASCII letters.
[[nodiscard]] bool
equalsIgnoringCase(std::string_view a, std::string_view b) noexcept
{
    return std::ranges::equal(a, b, [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
    });
}

/// Displays text wrapped to the width of the current layout.
void wrappedText(devgui::UI& ui, devgui::Key key, std::string_view text)
{
    const auto& style = ui.getStyle();
    const auto width = ui.getAvailableWidth();
    const auto wrapped = ui.wrapText(ui.makeID(key), text, width, i18n::LineBreakLanguage::Default);
    const auto lineHeight = style.getLineHeight();
    const auto lineCount = std::ranges::count(wrapped, '\n') + 1;
    const auto rect = ui.allocate(width, lineHeight * static_cast<f32>(lineCount));

    // NOTE: Draw only the lines inside the scrolled view.
    auto lineTop = rect.y;
    std::string_view rest = wrapped;
    for (;;) {
        const auto end = rest.find('\n');
        const auto line = rest.substr(0, end);
        if (ui.isVisible(F32Rect2D{rect.x, lineTop, rect.width, lineHeight})) {
            ui.drawText(Vector2{rect.x, lineTop}, line, style.textColor);
        }
        if (end == std::string_view::npos) {
            break;
        }
        rest.remove_prefix(end + 1);
        lineTop += lineHeight;
    }
}

} // namespace

HTTPClientTest::HTTPClientTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
HTTPClientTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: GUI
    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    sendRequest();

    return nullptr;
}

void HTTPClientTest::sendRequest()
{
    loading_ = true;
    status_ = "Loading...";
    contentType_.clear();
    bodySize_.clear();
    body_.clear();

    auto callback = [this](std::unique_ptr<HTTPResponse>&& resp, const std::unique_ptr<Error>& err) {
        loading_ = false;
        if (err != nullptr) {
            status_ = "Failed";
            body_ = err->toString();
            return;
        }
        status_ = pomdog::format("{} {}", resp->Protocol, resp->Status);
        for (const auto& [name, value] : resp->Header) {
            if (equalsIgnoringCase(name, "Content-Type")) {
                contentType_ = value;
            }
        }
        bodySize_ = pomdog::format("{} bytes", resp->Body.size());
        body_.assign(resp->Body.data(), resp->Body.size());
    };

    // NOTE: Assigning the new connection disconnects the request in flight,
    // so that its response does not overwrite this one.
    const auto url = std::string{requestURLs[static_cast<std::size_t>(selectedURL_)]};
    auto [conn, err] = gameHost_->getHTTPClient()->Get(url, std::move(callback));
    if (err != nullptr) {
        loading_ = false;
        status_ = "Failed";
        body_ = err->toString();
        return;
    }
    connection_ = std::move(conn);
}

void HTTPClientTest::buildUI(devgui::UI& ui)
{
    using namespace pomdog::devgui::literals;

    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{560.0f, 400.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "httpClient"_key, "HTTP Client", options);
    if (!window.isOpen()) {
        return;
    }

    devgui::combo(ui, "request"_key, "Request", selectedURL_, requestNames);
    devgui::text(ui, requestURLs[static_cast<std::size_t>(selectedURL_)], ui.getStyle().textSecondaryColor);
    if (devgui::button(ui, "send"_key, "Send GET Request").clicked) {
        sendRequest();
    }

    devgui::separator(ui);

    devgui::labelText(ui, "Status", status_);
    devgui::labelText(ui, "Content-Type", contentType_);
    devgui::labelText(ui, "Body", bodySize_);

    auto area = devgui::scrollArea(ui, "body"_key, 200.0f);
    wrappedText(ui, "bodyText"_key, body_);
}

void HTTPClientTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });
}

void HTTPClientTest::draw()
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
