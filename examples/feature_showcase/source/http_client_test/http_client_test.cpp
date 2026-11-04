#include "http_client_test.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cctype>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

namespace {

// NOTE: These servers allow requests from other origins (CORS), which the
// browser requires on the web. A page served over https cannot send the plain
// HTTP request.
constexpr auto requestNames = std::to_array<std::string_view>({
    "LICENSE.md (GitHub)",
    "Missing file (GitHub)",
    "JSON (httpbin.org)",
    "Plain HTTP (httpbin.org)",
});

constexpr auto requestURLs = std::to_array<std::string_view>({
    "https://raw.githubusercontent.com/mogemimi/pomdog/main/LICENSE.md",
    "https://raw.githubusercontent.com/mogemimi/pomdog/main/not_found.txt",
    "https://httpbin.org/get",
    "http://httpbin.org/get",
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

HTTPClientTest::~HTTPClientTest() = default;

std::unique_ptr<Error>
HTTPClientTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Read the root certificates by hash key, which reads the archive
    // only. A file in the overlay directory cannot replace the roots then.
    auto [pemFile, pemFileErr] = vfs::open(
        fs_,
        computeStringHash64("/assets"),
        computeStringHash64("certificates/root_certificates.pem"));
    if (pemFileErr != nullptr) {
        return errors::wrap(std::move(pemFileErr), "failed to open root certificates");
    }
    auto [pem, pemErr] = pemFile->view();
    if (pemErr != nullptr) {
        return errors::wrap(std::move(pemErr), "failed to read root certificates");
    }

    network::HTTPClientOptions clientOptions;
    if (auto trustStore = network::createTrustStoreFromPEM(pem); !trustStore) {
        return errors::wrap(std::move(trustStore.error()), "failed to create trust store");
    }
    else {
        clientOptions.trustStore = std::move(*trustStore);
    }

    if (auto httpClient = network::createHTTPClient(clientOptions); !httpClient) {
        return errors::wrap(std::move(httpClient.error()), "failed to create HTTPClient");
    }
    else {
        httpClient_ = std::move(*httpClient);
    }

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
    status_ = "Loading...";
    contentType_.clear();
    bodySize_.clear();
    body_.clear();

    // NOTE: Cancel the request in flight, so that its response does not
    // overwrite this one.
    if (requestID_ != 0) {
        httpClient_->cancel(requestID_);
    }

    network::HTTPRequest request;
    request.method = network::HTTPMethod::Get;
    request.url = requestURLs[static_cast<std::size_t>(selectedURL_)];
    requestID_ = httpClient_->send(std::move(request));
}

void HTTPClientTest::receiveResponses()
{
    httpClient_->update();

    while (auto event = httpClient_->pollEvent()) {
        if (event->requestID != requestID_) {
            continue;
        }
        requestID_ = 0;

        if (!event->result) {
            status_ = "Failed";
            body_ = event->result.error()->toString();
            continue;
        }

        const auto& response = *event->result;
        // NOTE: The protocol is empty on the web, where fetch() does not tell it.
        status_ = response.protocol.empty() ? response.status : pomdog::format("{} {}", response.protocol, response.status);
        for (const auto& [name, value] : response.headers) {
            if (equalsIgnoringCase(name, "Content-Type")) {
                contentType_ = value;
            }
        }
        bodySize_ = pomdog::format("{} bytes", response.body.size());
        body_.assign(reinterpret_cast<const char*>(response.body.data()), response.body.size());
    }
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
    // NOTE: Take the responses before building the UI that shows them.
    receiveResponses();

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
