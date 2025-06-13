#include "PlayerCharacterComponent.h"

#include <algorithm> 
#include <iostream>  
#include <typeinfo>  

#include "GameObject.h"
#include "TransformComponent.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "StateMachineComponent.h"
#include "PlayerRunState.h"  
#include "PlayerIdleState.h" 
#include "ServiceLocator.h"
#include "ISoundService.h"   // For the type ISoundService
#include "PlayerCommands.h" 
#include "BoxCollisionComponent.h" 
#include "Scene.h"                 

namespace dae
{
    PlayerCharacterComponent::PlayerCharacterComponent(GameObject* owner, float speed)
        : BaseComponent(owner),
        m_Speed(speed),
        m_Direction(0.0f, 0.0f),
        m_VerticalVelocity(0.0f),
        m_IsOnGround(false),
        m_JumpStrength(600.0f),
        m_Gravity(981.0f),
        m_pTransform(nullptr),
        m_pPlayerCollider(nullptr),
        m_pStateMachine(nullptr),
        m_pSoundService(nullptr),
        m_pCurrentScene(nullptr)
    {
        // Assuming ServiceLocator::GetSoundService() returns std::shared_ptr<ISoundService> (global)
        std::shared_ptr<ISoundService> soundServiceShared = ServiceLocator::GetSoundService();
        if (soundServiceShared)
        {
            m_pSoundService = soundServiceShared.get();
        }

        if (GetOwner())
        {
            auto transformPtr = GetOwner()->GetComponent<TransformComponent>();
            if (transformPtr) m_pTransform = transformPtr.get();
            else std::cerr << "[PCC::Constructor] Owner missing TransformComponent." << std::endl;

            auto colliderPtr = GetOwner()->GetComponent<BoxCollisionComponent>();
            if (colliderPtr) m_pPlayerCollider = colliderPtr.get();
            else std::cerr << "[PCC::Constructor] Owner missing BoxCollisionComponent." << std::endl;
        }
        else
        {
            std::cerr << "[PCC::Constructor] Owner GameObject is null." << std::endl;
        }
        EnsureStateMachine();
    }

    void PlayerCharacterComponent::SetCurrentScene(Scene* scene)
    {
        m_pCurrentScene = scene;
    }

    void PlayerCharacterComponent::Update(float deltaTime)
    {
        if (!GetOwner()) return;

        if (!m_pTransform)
        {
            auto transformPtr = GetOwner()->GetComponent<TransformComponent>();
            if (transformPtr) m_pTransform = transformPtr.get();
            else { return; }
        }
        if (!m_pPlayerCollider && GetOwner())
        {
            auto colliderPtr = GetOwner()->GetComponent<BoxCollisionComponent>();
            if (colliderPtr) m_pPlayerCollider = colliderPtr.get();
        }

        float horizontalMovement = 0.0f;
        if (std::abs(m_Direction.x) > 0.01f)
        {
            horizontalMovement = m_Direction.x * m_Speed * deltaTime;
        }

        if (!m_IsOnGround)
        {
            m_VerticalVelocity += m_Gravity * deltaTime;
        }

        glm::vec2 currentPos = m_pTransform->GetPosition();
        glm::vec2 potentialPos = currentPos;
        potentialPos.x += horizontalMovement;
        potentialPos.y += m_VerticalVelocity * deltaTime;

        m_IsOnGround = false;

        m_pTransform->SetPosition(potentialPos.x, potentialPos.y);
        if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(potentialPos.x, potentialPos.y);

        HandleCollisions(deltaTime);

        EnsureStateMachine();
        if (m_pStateMachine)
        {
            if (m_IsOnGround)
            {
                if (std::abs(m_Direction.x) > 0.01f)
                {
                    if (!m_pStateMachine->GetCurrentState() || typeid(*m_pStateMachine->GetCurrentState()) != typeid(PlayerRunState))
                    {
                        m_pStateMachine->ChangeState(std::make_unique<PlayerRunState>());
                    }
                }
                else
                {
                    if (!m_pStateMachine->GetCurrentState() || typeid(*m_pStateMachine->GetCurrentState()) != typeid(PlayerIdleState))
                    {
                        m_pStateMachine->ChangeState(std::make_unique<PlayerIdleState>());
                    }
                }
            }
        }
    }

    void PlayerCharacterComponent::HandleCollisions(float /*deltaTime*/)
    {
        if (!GetOwner() || !m_pPlayerCollider || !m_pTransform) return;

        if (!m_pCurrentScene)
        {
            glm::vec2 currentPosAfterTentativeMove = m_pTransform->GetPosition();
            const float fallbackGroundY = 676.0f;
            if (currentPosAfterTentativeMove.y >= fallbackGroundY)
            {
                glm::vec2 finalPos = currentPosAfterTentativeMove;
                finalPos.y = fallbackGroundY;
                m_VerticalVelocity = 0.0f;
                m_IsOnGround = true;
                m_pTransform->SetPosition(finalPos.x, finalPos.y);
                if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(finalPos.x, finalPos.y);
            }
            return;
        }

        AABB playerAABB = m_pPlayerCollider->GetBoundingBox();
        glm::vec2 resolvedPlayerPos = m_pTransform->GetPosition();
        bool landedThisFrame = false;

        const auto& gameObjects = m_pCurrentScene->GetAllGameObjects();

        for (const auto& otherGameObjectSharedPtr : gameObjects)
        {
            if (!otherGameObjectSharedPtr || otherGameObjectSharedPtr.get() == GetOwner()) continue;

            GameObject* otherGameObjectPtr = otherGameObjectSharedPtr.get();
            auto otherColliderSharedPtr = otherGameObjectPtr->GetComponent<BoxCollisionComponent>();

            if (!otherColliderSharedPtr) continue;

            BoxCollisionComponent* otherColliderRawPtr = otherColliderSharedPtr.get();
            AABB otherAABB = otherColliderRawPtr->GetBoundingBox();

            if (m_pPlayerCollider->IsColliding(*otherColliderRawPtr, m_VerticalVelocity))
            {
                ColliderTag otherTag = otherColliderRawPtr->GetTag();

                if (m_VerticalVelocity >= 0.f &&
                    (otherTag == ColliderTag::SMALL_TILE || otherTag == ColliderTag::BIG_TILE))
                {
                    float playerFeet = playerAABB.GetBottom();
                    float tileTop = otherAABB.GetTop();

                    if (playerFeet >= tileTop && (playerAABB.GetTop() < tileTop + playerAABB.height * 0.5f))
                    {
                        bool horizontalOverlap = (playerAABB.GetLeft() < otherAABB.GetRight() &&
                            playerAABB.GetRight() > otherAABB.GetLeft());
                        if (horizontalOverlap)
                        {
                            resolvedPlayerPos.y = tileTop - playerAABB.height;
                            m_VerticalVelocity = 0.0f;
                            landedThisFrame = true;
                        }
                    }
                }
            }
        }

        if (landedThisFrame)
        {
            m_IsOnGround = true;
            m_pTransform->SetPosition(resolvedPlayerPos.x, resolvedPlayerPos.y);
            if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(resolvedPlayerPos.x, resolvedPlayerPos.y);
        }
        else
        {
            glm::vec2 currentPosAfterCollisionLogic = m_pTransform->GetPosition();
            const float fallbackGroundY = 676.0f;
            if (currentPosAfterCollisionLogic.y >= fallbackGroundY)
            {
                currentPosAfterCollisionLogic.y = fallbackGroundY;
                m_VerticalVelocity = 0.0f;
                m_IsOnGround = true;
                m_pTransform->SetPosition(currentPosAfterCollisionLogic.x, currentPosAfterCollisionLogic.y);
                if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(currentPosAfterCollisionLogic.x, currentPosAfterCollisionLogic.y);
            }
        }
    }

    void PlayerCharacterComponent::Render() const {}

    void PlayerCharacterComponent::Move(float x, float /*y*/)
    {
        m_Direction.x += x;
        m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
    }

    void PlayerCharacterComponent::StopMove(float x, float /*y*/)
    {
        m_Direction.x -= x;
        m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
    }

    void PlayerCharacterComponent::Jump()
    {
        if (m_IsOnGround)
        {
            m_VerticalVelocity = -m_JumpStrength;
            m_IsOnGround = false;
            if (m_pSoundService) { /* m_pSoundService->Play("JumpSoundID", 0.5f); */ }
        }
    }

    void PlayerCharacterComponent::DoDamage(int amount)
    {
        if (!GetOwner()) return;
        auto healthComponent = GetOwner()->GetComponent<HealthComponent>();
        if (healthComponent)
        {
            healthComponent->TakeDamage(amount);
            if (m_pSoundService) { /* m_pSoundService->Play("PlayerDamageSoundID", 0.5f); */ }
        }
    }

    void PlayerCharacterComponent::AddScore(int points)
    {
        if (!GetOwner()) return;
        auto scoreComponent = GetOwner()->GetComponent<ScoreComponent>();
        if (scoreComponent)
        {
            scoreComponent->AddScore(points);
            if (m_pSoundService) { /* m_pSoundService->Play("CollectPointSoundID", 0.5f); */ }
        }
    }

    void PlayerCharacterComponent::EnsureStateMachine()
    {
        if (!m_pStateMachine)
        {
            if (GetOwner())
            {
                auto sm = GetOwner()->GetComponent<StateMachineComponent>();
                if (sm)
                {
                    m_pStateMachine = sm.get();
                    if (m_pStateMachine && !m_pStateMachine->GetCurrentState() && m_IsOnGround)
                    {
                        m_pStateMachine->ChangeState(std::make_unique<PlayerIdleState>());
                    }
                }
            }
        }
    }

    void PlayerCharacterComponent::BindInputs(bool isKeyboard, int playerNumberForInput)
    {
        std::cout << "[PCC::BindInputs] Called. isKeyboard: " << (isKeyboard ? "true" : "false")
            << ", playerNumberForInput: " << playerNumberForInput << std::endl;

        InputManager& inputManager = InputManager::GetInstance();
        std::shared_ptr<PlayerCharacterComponent> sharedThis = GetOwner()->GetComponent<PlayerCharacterComponent>();

        if (!sharedThis)
        {
            std::cerr << "[PCC::BindInputs] Error: Owner is missing PlayerCharacterComponent (self)." << std::endl;
            return;
        }

        if (isKeyboard)
        {
            std::cout << "  [PCC::BindInputs] Binding keyboard commands." << std::endl;
            inputManager.BindCommand(SDLK_a, InputState::Down, std::make_shared<MoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindCommand(SDLK_d, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 1.0f, 0.0f));
            inputManager.BindCommand(SDLK_a, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindCommand(SDLK_d, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 1.0f, 0.0f));
            inputManager.BindCommand(SDLK_SPACE, InputState::Pressed, std::make_shared<JumpCommand>(sharedThis));
            inputManager.BindCommand(SDLK_c, InputState::Pressed, std::make_shared<DamageCommand>(sharedThis, 1));
            inputManager.BindCommand(SDLK_z, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 10));
            inputManager.BindCommand(SDLK_x, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 100));
        }
        else // Controller
        {
            std::cout << "  [PCC::BindInputs] Binding controller commands for playerNumberForInput: " << playerNumberForInput << std::endl;
            if (playerNumberForInput < 1)
            {
                std::cerr << "  [PCC::BindInputs] Warning: playerNumberForInput " << playerNumberForInput
                    << " might not be handled correctly by the current 3-argument BindControllerCommand if multiple controllers need distinct bindings." << std::endl;
            }

            inputManager.BindControllerCommand(GamepadButton::DPadLeft, InputState::Down, std::make_shared<MoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadRight, InputState::Down, std::make_shared<MoveCommand>(sharedThis, 1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadLeft, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, -1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::DPadRight, InputState::Released, std::make_shared<StopMoveCommand>(sharedThis, 1.0f, 0.0f));
            inputManager.BindControllerCommand(GamepadButton::ButtonA, InputState::Pressed, std::make_shared<JumpCommand>(sharedThis));
            inputManager.BindControllerCommand(GamepadButton::ButtonX, InputState::Pressed, std::make_shared<DamageCommand>(sharedThis, 1));
            inputManager.BindControllerCommand(GamepadButton::ButtonY, InputState::Pressed, std::make_shared<ScoreCommand>(sharedThis, 10));
        }
    }

} // namespace dae