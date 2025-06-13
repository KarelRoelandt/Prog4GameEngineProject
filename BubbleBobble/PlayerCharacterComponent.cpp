#include "PlayerCharacterComponent.h"

#include <algorithm> 
#include <iostream>  
#include <typeinfo>  
#include <iomanip> 

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
        m_JumpStrength(550.0f),
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







    void dae::PlayerCharacterComponent::HandleCollisions(float /*deltaTime*/)
    {
        if (!GetOwner())
        {
            std::cout << "[CollisionDebug] HandleCollisions: GetOwner() is null." << std::endl;
            return;
        }
        if (!m_pPlayerCollider)
        {
            std::cout << "[CollisionDebug] HandleCollisions: m_pPlayerCollider is null." << std::endl;
            return;
        }
        if (!m_pTransform)
        {
            std::cout << "[CollisionDebug] HandleCollisions: m_pTransform is null." << std::endl;
            return;
        }
        if (!m_pCurrentScene)
        {
            std::cout << "[CollisionDebug] HandleCollisions: m_pCurrentScene is null. Using fallback ground logic." << std::endl;

            glm::vec2 currentPos = m_pTransform->GetPosition();
            float groundY = 600.0f - m_pPlayerCollider->GetBoundingBox().height;
            if (currentPos.y >= groundY && m_VerticalVelocity >= 0)
            {
                currentPos.y = groundY;
                m_pTransform->SetPosition(currentPos.x, currentPos.y);
                m_pPlayerCollider->SetPosition(currentPos.x, currentPos.y);
                m_VerticalVelocity = 0.0f;
                m_IsOnGround = true;
            }
            else
            {
                m_IsOnGround = false;
            }
            return;
        }

        std::cout << "[CollisionDebug] HandleCollisions: m_pCurrentScene is OK. Starting main collision checks." << std::endl;

        bool landedThisFrame = false;
        glm::vec2 resolvedPlayerPos = m_pTransform->GetPosition();
        AABB playerAABB = m_pPlayerCollider->GetBoundingBox();

        const auto& gameObjects = m_pCurrentScene->GetAllGameObjects();
        for (const auto& otherGameObject : gameObjects)
        {
            if (otherGameObject.get() == GetOwner()) continue;

            auto otherCollider = otherGameObject->GetComponent<BoxCollisionComponent>();
            if (!otherCollider) continue;

            BoxCollisionComponent* otherColliderRawPtr = otherCollider.get();
            AABB otherAABB = otherColliderRawPtr->GetBoundingBox();
            ColliderTag otherTag = otherColliderRawPtr->GetTag();

            if (m_pPlayerCollider->IsColliding(*otherColliderRawPtr, m_VerticalVelocity))
            {
                std::cout << "[CollisionDebug] IsColliding/Overlap TRUE with object tagged: " << static_cast<int>(otherTag)
                    << " (Player VelY: " << m_VerticalVelocity << ")" << std::endl;
                std::cout << "  Player AABB - L: " << playerAABB.GetLeft() << " T: " << playerAABB.GetTop()
                    << " R: " << playerAABB.GetRight() << " B: " << playerAABB.GetBottom()
                    << " W: " << playerAABB.width << " H: " << playerAABB.height << std::endl;
                std::cout << "  Other AABB  - L: " << otherAABB.GetLeft() << " T: " << otherAABB.GetTop()
                    << " R: " << otherAABB.GetRight() << " B: " << otherAABB.GetBottom()
                    << " W: " << otherAABB.width << " H: " << otherAABB.height << " (Tag: " << static_cast<int>(otherTag) << ")" << std::endl;

                glm::vec2 currentPosition = m_pTransform->GetPosition();
                glm::vec2 playerCenter = { playerAABB.GetLeft() + playerAABB.width / 2.0f, playerAABB.GetTop() + playerAABB.height / 2.0f };
                glm::vec2 tileCenter = { otherAABB.GetLeft() + otherAABB.width / 2.0f, otherAABB.GetTop() + otherAABB.height / 2.0f };

                float deltaX = playerCenter.x - tileCenter.x;
                float deltaY = playerCenter.y - tileCenter.y;

                float combinedHalfWidths = playerAABB.width / 2.0f + otherAABB.width / 2.0f;
                float combinedHalfHeights = playerAABB.height / 2.0f + otherAABB.height / 2.0f;

                float overlapX = combinedHalfWidths - std::abs(deltaX);
                float overlapY = combinedHalfHeights - std::abs(deltaY);

                std::cout << "    OverlapX: " << overlapX << ", OverlapY: " << overlapY << std::endl;

                if (otherTag == ColliderTag::BIG_TILE)
                {
                    std::cout << "    [CollisionDebug] BIG_TILE collision." << std::endl;
                    if (overlapX < overlapY && overlapX > 0)
                    {
                        std::cout << "      Resolving X axis. Player VelX related to m_Direction.x: " << m_Direction.x * m_Speed << std::endl;
                        if (deltaX > 0)
                        {
                            currentPosition.x += overlapX;
                            std::cout << "        Pushing player RIGHT to " << currentPosition.x << std::endl;
                        }
                        else
                        {
                            currentPosition.x -= overlapX;
                            std::cout << "        Pushing player LEFT to " << currentPosition.x << std::endl;
                        }
                    }
                    else if (overlapY > 0)
                    {
                        std::cout << "      Resolving Y axis. Player VelY = " << m_VerticalVelocity << std::endl;
                        if (deltaY > 0)
                        {
                            currentPosition.y += overlapY;
                            std::cout << "        Pushing player DOWN to " << currentPosition.y << std::endl;
                            if (m_VerticalVelocity < 0) m_VerticalVelocity = 0;
                        }
                        else
                        {
                            currentPosition.y -= overlapY;
                            std::cout << "        Pushing player UP to " << currentPosition.y << std::endl;
                            if (m_VerticalVelocity >= 0)
                            {
                                currentPosition.y = otherAABB.GetTop() - playerAABB.height;
                                std::cout << "          Refined Y for landing: " << currentPosition.y << std::endl;
                                m_VerticalVelocity = 0.0f;
                                landedThisFrame = true;
                            }
                        }
                    }
                    m_pTransform->SetPosition(currentPosition.x, currentPosition.y);
                    if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(currentPosition.x, currentPosition.y);
                    playerAABB = m_pPlayerCollider->GetBoundingBox();
                    break;
                }
                else if (otherTag == ColliderTag::SMALL_TILE)
                {
                    std::cout << "    [CollisionDebug] SMALL_TILE collision." << std::endl;
                    if (m_VerticalVelocity >= 0.f)
                    {
                        float playerFeet = playerAABB.GetBottom();
                        float tileTop = otherAABB.GetTop();
                        if (playerFeet >= tileTop && (playerAABB.GetTop() < tileTop + playerAABB.height * 0.5f))
                        {
                            bool horizontalOverlap = (playerAABB.GetLeft() < otherAABB.GetRight() &&
                                playerAABB.GetRight() > otherAABB.GetLeft());
                            if (horizontalOverlap)
                            {
                                std::cout << "      SMALL_TILE: Player LANDING. TileTop: " << tileTop << ", PlayerHeight: " << playerAABB.height << std::endl;
                                resolvedPlayerPos.y = tileTop - playerAABB.height;
                                m_VerticalVelocity = 0.0f;
                                landedThisFrame = true;
                                std::cout << "        New Y: " << resolvedPlayerPos.y << std::endl;
                                m_pTransform->SetPosition(resolvedPlayerPos.x, resolvedPlayerPos.y);
                                if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(resolvedPlayerPos.x, resolvedPlayerPos.y);
                                playerAABB = m_pPlayerCollider->GetBoundingBox();
                                break;
                            }
                        }
                    }
                }
            }
        }
        m_IsOnGround = landedThisFrame;
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