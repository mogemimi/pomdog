#include "TestGame.hpp"
#include <utility>
#include <cmath>

namespace Pomdog {

TestGame::TestGame(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , gameEngine(gameHostIn)
    , inputStream(gameHostIn)
{
}

void TestGame::Initialize()
{
    auto & entityManager = gameEngine.entityManager;
    {
        // Camera
        auto entity = entityManager.CreateEntity({
            AddComponent<Transform>(),
            AddComponent<CameraComponent>()
        });

        auto camera = entity.GetComponent<CameraComponent>();
        camera->SetBackgroundColor(Color{32, 31, 30, 255});
        camera->SetProjectionType(ProjectionType::Perspective);
        camera->SetNear(0.1f);
        camera->SetFar(1000.0f);
        camera->SetFieldOfView(MathHelper::ToRadians<float>(45.0f));

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition(Vector3{0, 0, -512.0f});
        transform->SetRotationX(MathHelper::ToRadians<float>(10.0f));

        gameEngine.SetCamera(entity);

        auto clock = gameHost->GetClock();
        auto input = gameHost->GetKeyboard();

        static bool isUp = false;
        static bool isDown = false;
        static bool isRight = false;
        static bool rotateY1 = false;
        static bool rotateY2 = false;
        connect(input->KeyDown, [this](Keys key) {
            if (key == Keys::A) {
                inputStream.SetEnable(true);
            }
            if (key == Keys::D) {
                isRight = true;
            }
            if (key == Keys::W) {
                isUp = true;
            }
            if (key == Keys::S) {
                isDown = true;
            }
            if (key == Keys::J) {
                rotateY1 = true;
            }
            if (key == Keys::L) {
                rotateY2 = true;
            }
        });
        connect(input->KeyUp, [this](Keys key) {
            if (key == Keys::A) {
                inputStream.SetEnable(false);
            }
            if (key == Keys::D) {
                isRight = false;
            }
            if (key == Keys::W) {
                isUp = false;
            }
            if (key == Keys::S) {
                isDown = false;
            }
            if (key == Keys::J) {
                rotateY1 = false;
            }
            if (key == Keys::L) {
                rotateY2 = false;
            }
        });

        connect += inputStream.Subscribe([transform, this] {
            auto frameDuration = gameHost->GetClock()->GetFrameDuration();
            transform->SetPosition(
                transform->GetPosition() +
                100.0f * static_cast<float>(frameDuration.count())
                * Vector3::Transform(-Vector3::UnitX, Matrix4x4::CreateFromQuaternion(transform->GetRotation())));
        });
        connect(clock->OnTick, [transform](const Duration& frameDuration) {
            if (isRight) {
                transform->SetPosition(
                    transform->GetPosition() +
                    100.0f * static_cast<float>(frameDuration.count())
                    * Vector3::Transform(Vector3::UnitX, Matrix4x4::CreateFromQuaternion(transform->GetRotation())));
            }
            if (isUp) {
                transform->SetPosition(
                    transform->GetPosition() +
                    100.0f * static_cast<float>(frameDuration.count())
                    * Vector3::Transform(Vector3::UnitZ, Matrix4x4::CreateFromQuaternion(transform->GetRotation())));
            }
            if (isDown) {
                transform->SetPosition(
                    transform->GetPosition() +
                    100.0f * static_cast<float>(frameDuration.count())
                    * Vector3::Transform(-Vector3::UnitZ, Matrix4x4::CreateFromQuaternion(transform->GetRotation())));
            }
            if (rotateY1) {
                transform->Rotate(Vector3(0, -frameDuration.count(), 0));
            }
            if (rotateY2) {
                transform->Rotate(Vector3(0, frameDuration.count(), 0));
            }
        });
    }
    {
        // Primitive
        auto entity = entityManager.CreateEntity({
            AddComponent<Transform>(),
            AddComponent<PrimitiveRenderable>(gameHost->GetGraphicsDevice())
        });

        auto renderable = entity.GetComponent<PrimitiveRenderable>();
        renderable->SetDrawOrder(1);
        renderable->DrawSphere(Vector3::Zero, 90.0f, Color::Red, 10);
        renderable->DrawSphere(Vector3{100, 100, 200}, 100.0f, Color::Black, 10);

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition(Vector3{0, 0, 1.0f});
        transform->SetRotationX(MathHelper::ToRadians<float>(15.0f));
    }
    {
        // Primitive
        auto entity = entityManager.CreateEntity({
            AddComponent<Transform>(),
            AddComponent<PrimitiveRenderable>(gameHost->GetGraphicsDevice()),
            AddComponent<ActorComponent>()
        });

        auto renderable = entity.GetComponent<PrimitiveRenderable>();
        renderable->SetDrawOrder(1);
        renderable->DrawSphere(Vector3{100, 0, 0}, 30.0f, Color::Yellow, 10);
        renderable->DrawSphere(Vector3{0, 0, -100}, 30.0f, Color::Green, 10);
        renderable->DrawSphere(Vector3{0, 0, 100}, 30.0f, Color::White, 10);
        renderable->DrawSphere(Vector3{-100, 0, 0}, 30.0f, Color::Blue, 10);

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition(Vector3{0, 0, 1.0f});
        transform->SetRotationX(MathHelper::ToRadians<float>(15.0f));

        auto actor = entity.GetComponent<ActorComponent>();
        actor->RunAction(std::make_unique<SequenceAction>(
            std::make_unique<ScaleToAction>(std::chrono::milliseconds(500), Vector3(1.5f, 2.5f, 1.5f)),
            std::make_unique<ScaleToAction>(std::chrono::milliseconds(800), 0.5f),
            std::make_unique<ScaleToAction>(std::chrono::milliseconds(500), Vector3(2.5f, 1.5f, 2.5f)),
            std::make_unique<ScaleToAction>(std::chrono::milliseconds(900), 1.0f)));
        actor->RunAction(std::make_unique<SequenceAction>(
            std::make_unique<RotateToAction>(std::chrono::seconds(40), 0, 10 * MathConstants<float>::TwoPi(), 0),
            std::make_unique<RotateToAction>(std::chrono::seconds(40), 0, 10 * MathConstants<float>::TwoPi(), 0),
            std::make_unique<RotateToAction>(std::chrono::seconds(40), 0, 10 * MathConstants<float>::TwoPi(), 0),
            std::make_unique<RotateToAction>(std::chrono::seconds(40), 0, 10 * MathConstants<float>::TwoPi(), 0)));
    }
    {
        // Sprite
        auto entity = entityManager.CreateEntity({
            AddComponent<Transform>(),
            AddComponent<SpriteRenderable>()
        });

        auto texture = gameHost->GetAssetManager()->Load<Texture2D>("pomdog.png");

        auto sprite = entity.GetComponent<SpriteRenderable>();
        sprite->SetDrawOrder(5);
        sprite->SetTexture(texture);
        sprite->SetTextureRegion({0, 0, texture->GetWidth(), texture->GetHeight()});

        auto transform = entity.GetComponent<Transform>();
        transform->SetPosition(Vector3{0, 0, 0.0f});
//        transform->SetRotationZ(0.0f);
    }
    Log::Verbose(StringHelper::Format("ok = %lu", entityManager.GetCount()));
}

void TestGame::Update()
{
    gameEngine.Update();
}

void TestGame::Draw()
{
    gameEngine.Draw();
}

} // namespace Pomdog
