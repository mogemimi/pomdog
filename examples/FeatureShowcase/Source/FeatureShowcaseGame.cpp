#include "FeatureShowcaseGame.hpp"
#include "PolygonBatchTest/PolygonBatchTest.hpp"
#include "SpriteBatchTest/SpriteBatchTest.hpp"
#include "SpriteFontTest/SpriteFontTest.hpp"
#include <cmath>
#include <utility>

namespace FeatureShowcase {

FeatureShowcaseGame::FeatureShowcaseGame(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , window(gameHostIn->GetWindow())
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , assets(gameHostIn->GetAssetManager())
    , clock(gameHostIn->GetClock())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void FeatureShowcaseGame::Initialize()
{
    window->SetTitle("FeatureShowcase");
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

    auto font = std::make_shared<TrueTypeFont>(*assets, "Fonts/NotoSans/NotoSans-Regular.ttf");
    spriteFont = std::make_shared<SpriteFont>(graphicsDevice, font, 20, 20);
    spriteBatch = std::make_shared<SpriteBatch>(graphicsDevice, *assets);
    spriteFont->PrepareFonts("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ012345689.,!?-+/():;%&`'*#=[]\" ");
    polygonBatch = std::make_shared<PolygonBatch>(graphicsDevice, *assets);
    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(1.0);

    buttons.emplace_back("PolygonBatch Test", [this] {
        subGame = std::make_shared<FeatureShowcase::PolygonBatchTest>(gameHost);
    });
    buttons.emplace_back("SpriteBatch Test", [this] {
        subGame = std::make_shared<FeatureShowcase::SpriteBatchTest>(gameHost);
    });
    buttons.emplace_back("SpriteFont Test", [this] {
        subGame = std::make_shared<FeatureShowcase::SpriteFontTest>(gameHost);
    });

    hudButtons.emplace_back("Back", [this] {
        subGame.reset();
    });

    auto mouse = gameHost->GetMouse();
    connect(mouse->ButtonDown, [this](MouseButtons mouseButton) {
        if (mouseButton != MouseButtons::Left) {
            return;
        }

        if (subGame) {
            for (auto& button : hudButtons) {
                if (button.Selected) {
                    button.OnClicked();
                }
            }
        }
        else {
            for (auto& button : buttons) {
                if (button.Selected) {
                    button.OnClicked();
                    if (subGame) {
                        subGame->Initialize();
                    }
                }
            }
        }
    });

    fpsTimer = std::make_shared<Timer>(clock);
    fpsTimer->SetInterval(std::chrono::milliseconds(150));
    connect(fpsTimer->Elapsed, [this] {
        footerString = StringHelper::Format("%.2f fps", clock->GetFrameRate());
    });
}

void FeatureShowcaseGame::Update()
{
    if (subGame) {
        subGame->Update();
    }

    UpdateMenuLayout();

    auto mouse = gameHost->GetMouse();
    auto mouseState = mouse->GetState();
    auto position = mouseState.Position;
    position.Y = window->GetClientBounds().Height - position.Y;

    if (subGame) {
        for (auto& button : hudButtons) {
            button.Selected = button.Rect.Contains(position);
        }
    }
    else {
        for (auto& button : buttons) {
            button.Selected = button.Rect.Contains(position);
        }
    }
}

void FeatureShowcaseGame::UpdateMenuLayout()
{
    {
        int y = window->GetClientBounds().Height - 36;
        constexpr int buttonVerticalMargin = 5;

        for (auto& button : buttons) {
            button.Rect.X = 10;
            button.Rect.Y = y;
            button.Rect.Width = 400;
            button.Rect.Height = 28;
            y -= (button.Rect.Height + buttonVerticalMargin);
        }
    }
    {
        int y = window->GetClientBounds().Height - 36;
        constexpr int buttonVerticalMargin = 5;

        for (auto& button : hudButtons) {
            button.Rect.X = 10;
            button.Rect.Y = y;
            button.Rect.Width = 80;
            button.Rect.Height = 28;
            y -= (button.Rect.Height + buttonVerticalMargin);
        }
    }
}

void FeatureShowcaseGame::Draw()
{
    commandQueue->Reset();

    if (subGame) {
        subGame->Draw();
    }

    DrawMenu();
    commandQueue->PushbackCommandList(commandList);

    commandQueue->ExecuteCommandLists();
    commandQueue->Present();
}

void FeatureShowcaseGame::DrawMenu()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    std::optional<Vector4> clearColor;
    if (!subGame) {
        clearColor = Color{60, 60, 60, 255}.ToVector4();
    }

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, clearColor);
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.BackBufferWidth,
        presentationParameters.BackBufferHeight,
        0.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::CreateTranslation(Vector3{
        static_cast<float>(-presentationParameters.BackBufferWidth) * 0.5f,
        static_cast<float>(-presentationParameters.BackBufferHeight) * 0.5f,
        0.0f});

    auto viewProjection = viewMatrix * projectionMatrix;

    auto drawButton = [this](const Button& button) {
        {
            Color color = Color{121, 120, 118, 255};
            if (button.Selected) {
                color = Color::Lerp(
                    Color{160, 160, 160, 255},
                    Color{191, 190, 180, 255},
                    0.5f + 0.5f * std::cos(Math::TwoPi<float> * static_cast<float>(timer->GetTotalTime().count())));
            }

            polygonBatch->DrawRectangle(
                Vector2{static_cast<float>(button.Rect.X), static_cast<float>(button.Rect.Y)},
                static_cast<float>(button.Rect.Width),
                static_cast<float>(button.Rect.Height),
                Vector2{0.0f, 0.0f}, color);
        }
        {
            Vector2 position = {static_cast<float>(button.Rect.X), static_cast<float>(button.Rect.Y)};

            Color color = Color{221, 220, 218, 255};
            if (button.Selected) {
                color = Color::White;
            }

            position.X += 10.0f;
            position.Y += 9.0f;

            spriteFont->Draw(*spriteBatch,
                button.Text,
                position,
                color,
                0.0f,
                Vector2{0.0f, 0.0f},
                1.0f);
        }
    };

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));
    polygonBatch->Begin(commandList, viewProjection);
    spriteBatch->Begin(commandList, viewProjection);
    if (subGame) {
        for (const auto& button : hudButtons) {
            drawButton(button);
        }

        spriteFont->Draw(*spriteBatch,
            footerString,
            Vector2{static_cast<float>(viewport.Width) - 8.0f, 8.0f},
            Color::White,
            0.0f,
            Vector2{1.0f, 0.0f},
            1.0f);
    }
    else {
        for (const auto& button : buttons) {
            drawButton(button);
        }
    }
    polygonBatch->End();
    spriteBatch->End();

    commandList->Close();
}

} // namespace FeatureShowcase
