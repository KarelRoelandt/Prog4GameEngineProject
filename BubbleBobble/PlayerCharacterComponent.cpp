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
        m_JumpStrength(500.0f),
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
            else std::cout << "[PCC::Constructor] Owner missing TransformComponent." << "\n";

            auto colliderPtr = GetOwner()->GetComponent<BoxCollisionComponent>();
            if (colliderPtr) m_pPlayerCollider = colliderPtr.get();
            else std::cout << "[PCC::Constructor] Owner missing BoxCollisionComponent." << "\n";
        }
        else
        {
            std::cout << "[PCC::Constructor] Owner GameObject is null." << "\n";
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
        if (!GetOwner() || !m_pPlayerCollider || !m_pTransform)
        {
            return;
        }

        if (!m_pCurrentScene)
        {
            glm::vec2 fallbackPos = m_pTransform->GetPosition();
            AABB playerBox = m_pPlayerCollider->GetBoundingBox();
            float groundY = 600.0f - playerBox.height;
            if (fallbackPos.y >= groundY && m_VerticalVelocity >= 0)
            {
                fallbackPos.y = groundY;
                m_pTransform->SetPosition(fallbackPos.x, fallbackPos.y);
                m_pPlayerCollider->SetPosition(fallbackPos.x, fallbackPos.y);
                m_VerticalVelocity = 0.0f;
                m_IsOnGround = true;
            }
            else
            {
                m_IsOnGround = false;
            }
            return;
        }

        bool landedThisFrame = false;
        const auto& gameObjects = m_pCurrentScene->GetAllGameObjects();

        for (const auto& otherGameObject : gameObjects)
        {
            if (otherGameObject.get() == GetOwner())
            {
                continue;
            }

            auto otherColliderComponent = otherGameObject->GetComponent<BoxCollisionComponent>();
            if (!otherColliderComponent)
            {
                continue;
            }

            BoxCollisionComponent* otherColliderRawPtr = otherColliderComponent.get();
            AABB otherAABB = otherColliderRawPtr->GetBoundingBox();
            ColliderTag otherTag = otherColliderRawPtr->GetTag();

            AABB playerAABB = m_pPlayerCollider->GetBoundingBox();
            glm::vec2 currentPosition = m_pTransform->GetPosition();

            if (m_pPlayerCollider->IsColliding(*otherColliderRawPtr, m_VerticalVelocity))
            {

                glm::vec2 playerCenter = { playerAABB.GetLeft() + playerAABB.width / 2.0f, playerAABB.GetTop() + playerAABB.height / 2.0f };
                glm::vec2 tileCenter = { otherAABB.GetLeft() + otherAABB.width / 2.0f, otherAABB.GetTop() + otherAABB.height / 2.0f };
                float deltaX = playerCenter.x - tileCenter.x;
                float deltaY = playerCenter.y - tileCenter.y;
                float combinedHalfWidths = playerAABB.width / 2.0f + otherAABB.width / 2.0f;
                float combinedHalfHeights = playerAABB.height / 2.0f + otherAABB.height / 2.0f;
                float overlapX = combinedHalfWidths - std::abs(deltaX);
                float overlapY = combinedHalfHeights - std::abs(deltaY);

                glm::vec2 resolvedPosition = currentPosition;
                bool collisionResolvedThisIteration = false;

                if (otherTag == ColliderTag::BIG_TILE)
                {
                    bool resolvedHorizontally = false;
                    if (overlapX > 0.001f && (overlapX < overlapY || overlapY <= 0.001f))
                    {
                        if (deltaX > 0)
                        {
                            resolvedPosition.x = currentPosition.x + overlapX;
                        }
                        else
                        {
                            resolvedPosition.x = currentPosition.x - overlapX;
                        }
                        resolvedHorizontally = true;
                        collisionResolvedThisIteration = true;
                    }

                    float epsilonResting = 0.5f; // Slightly larger epsilon for resting checks
                    bool restingOnBigTileTop = (!resolvedHorizontally && m_VerticalVelocity <= 0.f && // Not moving down significantly
                        (playerAABB.GetBottom() >= otherAABB.GetTop() - epsilonResting && playerAABB.GetBottom() <= otherAABB.GetTop() + epsilonResting) && // Feet near top
                        (playerAABB.GetLeft() < otherAABB.GetRight() - epsilonResting && playerAABB.GetRight() > otherAABB.GetLeft() + epsilonResting) && // Horizontal overlap
                        playerAABB.GetTop() < otherAABB.GetTop()); // Player is above

                    if ((overlapY > 0.001f && (!resolvedHorizontally || overlapY < overlapX)) || restingOnBigTileTop)
                    {
                        if (deltaY > 0 && !restingOnBigTileTop)
                        {
                            resolvedPosition.y = currentPosition.y + overlapY;
                            if (m_VerticalVelocity < 0) m_VerticalVelocity = 0;
                        }
                        else
                        {
                            resolvedPosition.y = otherAABB.GetTop() - playerAABB.height;
                            if (m_VerticalVelocity >= 0)
                            {
                                m_VerticalVelocity = 0.0f;
                                landedThisFrame = true;
                            }
                        }
                        collisionResolvedThisIteration = true;
                    }
                }
                else if (otherTag == ColliderTag::SMALL_TILE)
                {
                    bool landedOnSmallTileThisIteration = false;
                    glm::vec2 smallTileResolvedPosition = currentPosition;
                    float onTopEpsilon = 0.5f; // Epsilon for "on top" checks; 0.5 might be more robust than 0.1 for float issues

                    bool horizontalOverlap = (playerAABB.GetLeft() < otherAABB.GetRight() - onTopEpsilon &&
                        playerAABB.GetRight() > otherAABB.GetLeft() + onTopEpsilon);

                    if (horizontalOverlap)
                    {
                        if (m_VerticalVelocity > 0.f)
                        { // Player is falling
// Player is coming from above AND their feet are now at or have slightly passed the tile's top surface.
                            if (playerAABB.GetTop() < otherAABB.GetTop() &&
                                playerAABB.GetBottom() >= otherAABB.GetTop() - onTopEpsilon)
                            {

                                smallTileResolvedPosition.y = otherAABB.GetTop() - playerAABB.height;
                                m_VerticalVelocity = 0.0f;
                                landedOnSmallTileThisIteration = true;
                            }
                        }
                        else
                        { // m_VerticalVelocity <= 0.f (Player is stationary or moving up)
                                         // Player's feet are very close to the tile's top surface, and player is generally above the tile.
                            bool feetAreCorrectlyPositioned = (playerAABB.GetBottom() >= otherAABB.GetTop() - onTopEpsilon &&
                                playerAABB.GetBottom() <= otherAABB.GetTop() + onTopEpsilon);
                            bool playerIsAboveTile = playerAABB.GetTop() < otherAABB.GetTop();

                            if (feetAreCorrectlyPositioned && playerIsAboveTile)
                            {
                                smallTileResolvedPosition.y = otherAABB.GetTop() - playerAABB.height;
                                m_VerticalVelocity = 0.0f;
                                landedOnSmallTileThisIteration = true;
                            }
                        }
                    }

                    if (landedOnSmallTileThisIteration)
                    {
                        landedThisFrame = true;
                        resolvedPosition.y = smallTileResolvedPosition.y;
                        resolvedPosition.x = currentPosition.x; // For SMALL_TILE, only Y is resolved by this specific logic
                        collisionResolvedThisIteration = true;
                    }
                }

                if (collisionResolvedThisIteration)
                {
                    m_pTransform->SetPosition(resolvedPosition.x, resolvedPosition.y);
                    if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(resolvedPosition.x, resolvedPosition.y);
                }
            }
        }
        m_IsOnGround = landedThisFrame;

        AABB playerBoxForHardFloor = m_pPlayerCollider->GetBoundingBox();
        float hardFloorY = 724.0f - playerBoxForHardFloor.height;

        glm::vec2 finalPosition = m_pTransform->GetPosition();
        if (finalPosition.y > hardFloorY)
        {
            finalPosition.y = hardFloorY;
            m_pTransform->SetPosition(finalPosition.x, finalPosition.y);
            if (m_pPlayerCollider)
            {
                m_pPlayerCollider->SetPosition(finalPosition.x, finalPosition.y);
            }
            if (m_VerticalVelocity > 0)
            {
                m_VerticalVelocity = 0.0f;
            }
            m_IsOnGround = true;
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
            << ", playerNumberForInput: " << playerNumberForInput << "\n";

        InputManager& inputManager = InputManager::GetInstance();
        std::shared_ptr<PlayerCharacterComponent> sharedThis = GetOwner()->GetComponent<PlayerCharacterComponent>();

        if (!sharedThis)
        {
            std::cout << "[PCC::BindInputs] Error: Owner is missing PlayerCharacterComponent (self)." << "\n";
            return;
        }

        if (isKeyboard)
        {
            std::cout << "  [PCC::BindInputs] Binding keyboard commands." << "\n";
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
            std::cout << "  [PCC::BindInputs] Binding controller commands for playerNumberForInput: " << playerNumberForInput << "\n";
            if (playerNumberForInput < 1)
            {
                std::cout << "  [PCC::BindInputs] Warning: playerNumberForInput " << playerNumberForInput
                    << " might not be handled correctly by the current 3-argument BindControllerCommand if multiple controllers need distinct bindings." << "\n";
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