#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/io_service.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// HTTPClientTest sends a GET request with HTTPClient and shows the response
/// in a panel. GameHost does not provide network services, so this test owns
/// its IOService and steps it from update().
class HTTPClientTest final : public Game {
public:
    explicit HTTPClientTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    ~HTTPClientTest() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    void buildUI(devgui::UI& ui);

    void sendRequest();

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::unique_ptr<IOService> ioService_;
    std::unique_ptr<HTTPClient> httpClient_;
    ScopedConnection connection_;
    DevGUIOverlay overlay_;

    i32 selectedURL_ = 0;
    bool loading_ = false;
    std::string status_;
    std::string contentType_;
    std::string bodySize_;
    std::string body_;
};

} // namespace feature_showcase
