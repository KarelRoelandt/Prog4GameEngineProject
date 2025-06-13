// PlayerCharacterComponent.cpp

#include <algorithm>

#include "PlayerCharacterComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include "Controller.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "StateMachineComponent.h"
#include "PlayerRunState.h"
#include "PlayerIdleState.h"
#include "ServiceLocator.h"

#include "PlayerCommands.h"

namespace dae
{
    PlayerCharacterComponent::PlayerCharacterComponent(GameObject* owner, float speed)
        : BaseComponent(owner)
        , m_Speed(speed)
        , m_Direction(0.0f, 0.0f)
        , m_pTransform(nullptr)
        , m_pSoundService(ServiceLocator::GetSoundService())
        , m_pStateMachine(nullptr)
    {
        auto transformPtr = GetOwner()->GetComponent<TransformComponent>();
        m_pTransform = transformPtr.get();
    }

    void PlayerCharacterComponent::DoDamage(int amount)
    {
        auto healthComponent = GetOwner()->GetComponent<HealthComponent>();
        if (healthComponent)
        {
            healthComponent->TakeDamage(amount);
            m_pSoundService->LoadSound("Sound/KillEnemy.wav");
            m_pSoundService->OutputSound("Sound/KillEnemy.wav", 64);
        }
    }

    void PlayerCharacterComponent::AddScore(int points)
    {
        auto scoreComponent = GetOwner()->GetComponent<ScoreComponent>();
        if (scoreComponent)
        {
            scoreComponent->AddScore(points);
            m_pSoundService->LoadSound("Sound/GetFruit.wav");
            m_pSoundService->OutputSound("Sound/GetFruit.wav", 64);
        }
    }

    void PlayerCharacterComponent::Update(float deltaTime)
    {
        if (glm::length(m_Direction) > 0.1f)
        {
            glm::vec2 normalizedDir = glm::normalize(m_Direction);
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
        m_Direction.x += x;
        m_Direction.y += y;

        // Clamp direction to [-1, 1] for each axis
        m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
        m_Direction.y = std::clamp(m_Direction.y, -1.0f, 1.0f);

        EnsureStateMachine();
        if (m_pStateMachine &&
            (!m_pStateMachine->GetCurrentState() ||
                typeid(*m_pStateMachine->GetCurrentState()) != typeid(PlayerRunState)))
        {
            m_pStateMachine->ChangeState(std::make_unique<PlayerRunState>());
        }
    }

    void PlayerCharacterComponent::StopMove(float x, float y)
    {
        m_Direction.x -= x;
        m_Direction.y -= y;

        // Clamp direction to [-1, 1] for each axis
        m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
        m_Direction.y = std::clamp(m_Direction.y, -1.0f, 1.0f);

        EnsureStateMachine();
        if (m_pStateMachine &&
            (!m_pStateMachine->GetCurrentState() ||
                typeid(*m_pStateMachine->GetCurrentState()) != typeid(PlayerIdleState)))
        {
            // Only go idle if not moving
            if (glm::length(m_Direction) < 0.1f)
                m_pStateMachine->ChangeState(std::make_unique<PlayerIdleState>());
        }
    }

    void PlayerCharacterComponent::Jump()
    {
        std::cout << "[PlayerCharacterComponent] Jump triggered!\n";
    }

    void PlayerCharacterComponent::EnsureStateMachine()
    {
        if (!m_pStateMachine)
        {
            auto sm = GetOwner()->GetComponent<dae::StateMachineComponent>();
            m_pStateMachine = sm ? sm.get() : nullptr;
        }
    }

    void PlayerCharacterComponent::BindInputs(bool isKeyboard, int /*controllerIdx*/)
    {
        InputManager& inputManager = InputManager::GetInstance();
        // Get the real shared_ptr from the GameObject
        std::shared_ptr<PlayerCharacterComponent> sharedThis = GetOwner()->GetComponent<PlayerCharacterComponent>();

        if (isKeyboard)
        {
            inputManager.BindCommand(SDLK_a, InputState::Down, std::make_shared<dae::MoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindCommand(SDLK_d, InputState::Down, std::make_shared<dae::MoveCommand>(sharedThis, 1.0f, 0.0f));

            inputManager.BindCommand(SDLK_a, InputState::Released, std::make_shared<dae::StopMoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindCommand(SDLK_d, InputState::Released, std::make_shared<dae::StopMoveCommand>(sharedThis, 1.0f, 0.0f));

            inputManager.BindCommand(SDLK_c, InputState::Pressed, std::make_shared<dae::DamageCommand>(sharedThis, 1));
            inputManager.BindCommand(SDLK_z, InputState::Pressed, std::make_shared<dae::ScoreCommand>(sharedThis, 10));
            inputManager.BindCommand(SDLK_x, InputState::Pressed, std::make_shared<dae::ScoreCommand>(sharedThis, 100));
        }
        else
        {
            inputManager.BindControllerCommand(GamepadButton::DPadLeft, InputState::Down, std::make_shared<dae::MoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadRight, InputState::Down, std::make_shared<dae::MoveCommand>(sharedThis, 1.0f, 0.0f));

            inputManager.BindControllerCommand(GamepadButton::DPadLeft, InputState::Released, std::make_shared<dae::StopMoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadRight, InputState::Released, std::make_shared<dae::StopMoveCommand>(sharedThis, 1.0f, 0.0f));

            inputManager.BindControllerCommand(GamepadButton::ButtonA, InputState::Pressed, std::make_shared<dae::JumpCommand>(sharedThis));
            inputManager.BindControllerCommand(GamepadButton::ButtonX, InputState::Pressed, std::make_shared<dae::DamageCommand>(sharedThis, 1));
            inputManager.BindControllerCommand(GamepadButton::ButtonY, InputState::Pressed, std::make_shared<dae::ScoreCommand>(sharedThis, 10));
        }
    }

}