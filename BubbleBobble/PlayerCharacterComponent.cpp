// PlayerCharacterComponent.cpp
#include "PlayerCharacterComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include "Controller.h" // Include Controller.h to get access to the GamepadButton enum
#include "HealthComponent.h"
#include "ScoreComponent.h"

#include "ServiceLocator.h"

namespace dae
{
    PlayerCharacterComponent::PlayerCharacterComponent(GameObject* owner, float speed)
        : BaseComponent(owner)
        , m_Speed(speed)
        , m_MovingLeft(false)
        , m_MovingRight(false)
        , m_MovingUp(false)
        , m_MovingDown(false)
        , m_pSoundService(ServiceLocator::GetSoundService())
    {
        // Get the transform component as a raw pointer
        auto transformPtr = GetOwner()->GetComponent<TransformComponent>();
        m_pTransform = transformPtr.get();  // Use .get() to convert from shared_ptr to raw pointer
    }

    void PlayerCharacterComponent::DoDamage(int amount)
    {
        // Find the HealthComponent in the game object
        auto healthComponent = GetOwner()->GetComponent<HealthComponent>();
        if (healthComponent)
        {
            healthComponent->TakeDamage(amount);

            m_pSoundService->LoadSound("Data/Sound/KillEnemy.wav");
            m_pSoundService->OutputSound("Data/Sound/KillEnemy.wav", 64);
        }
    }

    void PlayerCharacterComponent::AddScore(int points)
    {
        // Find the ScoreComponent in the game object
        auto scoreComponent = GetOwner()->GetComponent<ScoreComponent>();
        if (scoreComponent)
        {
            scoreComponent->AddScore(points);

            m_pSoundService->LoadSound("Data/Sound/GetFruit.wav");
            m_pSoundService->OutputSound("Data/Sound/GetFruit.wav", 64);
        }
    }

    void PlayerCharacterComponent::Update(float deltaTime)
    {
        if (glm::length(m_Direction) > 0.1f)
        {
            // Normalize direction vector if it's not zero
            glm::vec2 normalizedDir = glm::normalize(m_Direction);

            // Update position using the transform component
            if (m_pTransform)
            {
                glm::vec2 position = m_pTransform->GetPosition();
                position.x += normalizedDir.x * m_Speed * deltaTime;
                position.y += normalizedDir.y * m_Speed * deltaTime;
                m_pTransform->SetPosition(position.x, position.y);
            }
        }
    }

    void PlayerCharacterComponent::Render() const
    {
        // Empty implementation or debug rendering if needed
    }

    void PlayerCharacterComponent::Move(float x, float y)
    {
        if (x < 0) m_MovingLeft = true;
        else if (x > 0) m_MovingRight = true;

        if (y < 0) m_MovingUp = true;
        else if (y > 0) m_MovingDown = true;

        UpdateDirection();
    }

    void PlayerCharacterComponent::StopMove(float x, float y)
    {
        if (x < 0) m_MovingLeft = false;
        else if (x > 0) m_MovingRight = false;

        if (y < 0) m_MovingUp = false;
        else if (y > 0) m_MovingDown = false;

        UpdateDirection();
    }

    void PlayerCharacterComponent::UpdateDirection()
    {
        m_Direction = { 0.0f, 0.0f };

        if (m_MovingLeft) m_Direction.x -= 1.0f;
        if (m_MovingRight) m_Direction.x += 1.0f;
        if (m_MovingUp) m_Direction.y -= 1.0f;
        if (m_MovingDown) m_Direction.y += 1.0f;
    }

    void PlayerCharacterComponent::BindInputs(bool isKeyboard, int /*controllerIdx*/)
    {
        InputManager& inputManager = InputManager::GetInstance();
        std::shared_ptr<PlayerCharacterComponent> sharedThis = std::shared_ptr<PlayerCharacterComponent>(this, [](PlayerCharacterComponent*) {});

        if (isKeyboard)
        {
            // Keyboard bindings
            // Pressing keys (movement)
            inputManager.BindCommand(SDLK_w, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 0.0f, -1.0f));
            inputManager.BindCommand(SDLK_a, InputState::Down, std::make_shared<MoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindCommand(SDLK_s, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 0.0f, 1.0f));
            inputManager.BindCommand(SDLK_d, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 1.0f, 0.0f));

            // Releasing keys (stop movement)
            inputManager.BindCommand(SDLK_w, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 0.0f, -1.0f));
            inputManager.BindCommand(SDLK_a, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindCommand(SDLK_s, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 0.0f, 1.0f));
            inputManager.BindCommand(SDLK_d, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 1.0f, 0.0f));

            // Add the C key binding for damage
            inputManager.BindCommand(SDLK_c, InputState::Pressed, std::make_shared<DamageCommand>(sharedThis, 1));

            // Add score key bindings
            inputManager.BindCommand(SDLK_z, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 10));
            inputManager.BindCommand(SDLK_x, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 100));
        }
        else
        {
            // Controller bindings
            // Pressing d-pad (movement)
            inputManager.BindControllerCommand(GamepadButton::DPadUp, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 0.0f, -1.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadLeft, InputState::Down, std::make_shared<MoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadDown, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 0.0f, 1.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadRight, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 1.0f, 0.0f));

            // Releasing d-pad (stop movement)
            inputManager.BindControllerCommand(GamepadButton::DPadUp, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 0.0f, -1.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadLeft, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadDown, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 0.0f, 1.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadRight, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 1.0f, 0.0f));

            // Add controller button binding for damage and score
            inputManager.BindControllerCommand(GamepadButton::ButtonX, InputState::Pressed, std::make_shared<DamageCommand>(sharedThis, 1));
            inputManager.BindControllerCommand(GamepadButton::ButtonA, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 10));
            inputManager.BindControllerCommand(GamepadButton::ButtonB, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 100));
        }
    }
}
