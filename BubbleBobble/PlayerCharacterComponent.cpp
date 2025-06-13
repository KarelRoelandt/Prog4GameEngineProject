#include "PlayerCharacterComponent.h" // Your header

#include <algorithm> // For std::clamp
#include <iostream>  // For debug output // <<< MAKE SURE THIS IS INCLUDED
#include <typeinfo>  // For typeid, if using it for state checks

#include "GameObject.h"
#include "TransformComponent.h"
#include "InputManager.h"
#include "Controller.h" // For GamepadButton, if used by InputManager
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "StateMachineComponent.h"
#include "PlayerRunState.h"  // Assuming you have these state classes
#include "PlayerIdleState.h" // Assuming you have these state classes
#include "ServiceLocator.h"
#include "ISoundService.h"   // Use ISoundService interface
#include "PlayerCommands.h" // For your command classes

// Includes for collision logic
#include "BoxCollisionComponent.h" // Defines AABB and ColliderTag (ensure ColliderTag is global or properly namespaced)
#include "SceneManager.h"          // For dae::SceneManager (if GetInstance() is static and gives access to scene)
#include "Scene.h"                 // For dae::Scene

namespace dae
{
    PlayerCharacterComponent::PlayerCharacterComponent(GameObject* owner, float speed)
        : BaseComponent(owner)
        , m_Speed(speed)
        // m_Direction, m_VerticalVelocity, m_IsOnGround, m_JumpStrength, m_Gravity,
        // m_pPlayerCollider, m_pTransform, m_pStateMachine are initialized in-class via your .h
        , m_pSoundService(ServiceLocator::GetSoundService())
    {
        std::cout << "[PlayerCharacterComponent::Constructor] Called for GameObject: "
            << (GetOwner() ? GetOwner()->GetName() : "NULL") // Assuming GetName() exists on GameObject for logging
            << " with speed: " << speed << std::endl;

        if (GetOwner())
        {
            auto transformPtr = GetOwner()->GetComponent<TransformComponent>();
            if (transformPtr) m_pTransform = transformPtr.get();
            else std::cerr << "[PlayerCharacterComponent::Constructor] Error: Owner GameObject is missing TransformComponent!" << std::endl;

            auto colliderPtr = GetOwner()->GetComponent<BoxCollisionComponent>();
            if (colliderPtr) m_pPlayerCollider = colliderPtr.get();
            else std::cerr << "[PlayerCharacterComponent::Constructor] Error: Owner GameObject is missing BoxCollisionComponent!" << std::endl;
        }
        else
        {
            std::cerr << "[PlayerCharacterComponent::Constructor] Error: Owner GameObject is null!" << std::endl;
        }
        EnsureStateMachine();
    }

    void PlayerCharacterComponent::Update(float deltaTime)
    {
        // Add a static counter to limit log frequency
        static int updateLogCounter = 0;
        const int logFrequency = 30; // Log roughly every half-second if running at 60 FPS

        bool shouldLogThisFrame = (updateLogCounter % logFrequency == 0);

        if (shouldLogThisFrame)
        {
            std::cout << "[PCC::Update START] Time: " << deltaTime
                << ", Pos: (" << (m_pTransform ? std::to_string(m_pTransform->GetPosition().x) : "N/A") << "," << (m_pTransform ? std::to_string(m_pTransform->GetPosition().y) : "N/A")
                << "), Dir.x: " << m_Direction.x << ", VVel: " << m_VerticalVelocity
                << ", Grounded: " << (m_IsOnGround ? "T" : "F") << std::endl;
        }

        if (!m_pTransform || !m_pPlayerCollider)
        {
            if (shouldLogThisFrame) std::cout << "[PCC::Update] Early exit: no transform or collider." << std::endl;
            updateLogCounter++;
            return;
        }

        float horizontalMovement = 0.0f;
        if (std::abs(m_Direction.x) > 0.01f)
        {
            horizontalMovement = m_Direction.x * m_Speed * deltaTime;
            if (shouldLogThisFrame) std::cout << "  [PCC::Update] HorizontalMovement calculated: " << horizontalMovement << " (Dir.x: " << m_Direction.x << ", Speed: " << m_Speed << ")" << std::endl;
        }

        if (!m_IsOnGround)
        {
            float prevVVel = m_VerticalVelocity;
            m_VerticalVelocity += m_Gravity * deltaTime;
            if (shouldLogThisFrame) std::cout << "  [PCC::Update] Applying gravity. VVel changed from " << prevVVel << " to " << m_VerticalVelocity << " (Grounded: F)" << std::endl;
        }
        else
        {
            if (shouldLogThisFrame && m_VerticalVelocity != 0.f) std::cout << "  [PCC::Update] Grounded, VVel is " << m_VerticalVelocity << " (should ideally be 0 or small if just landed)" << std::endl;
        }


        glm::vec2 currentPos = m_pTransform->GetPosition();
        glm::vec2 potentialPos = currentPos;
        potentialPos.x += horizontalMovement;
        potentialPos.y += m_VerticalVelocity * deltaTime;

        if (shouldLogThisFrame && (std::abs(horizontalMovement) > 0.0001f || std::abs(m_VerticalVelocity * deltaTime) > 0.0001f))
        {
            std::cout << "  [PCC::Update] Potential move: dX=" << horizontalMovement << ", dY=" << (m_VerticalVelocity * deltaTime)
                << ". OldPos: (" << currentPos.x << "," << currentPos.y << ")"
                << " -> NewPos: (" << potentialPos.x << "," << potentialPos.y << ")" << std::endl;
        }

        m_IsOnGround = false; // Assume not grounded, HandleCollisions will correct
        if (shouldLogThisFrame) std::cout << "  [PCC::Update] Set m_IsOnGround = false (before HandleCollisions)" << std::endl;


        m_pTransform->SetPosition(potentialPos.x, potentialPos.y);
        if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(potentialPos.x, potentialPos.y);
        if (shouldLogThisFrame) std::cout << "  [PCC::Update] Transform and Collider set to potentialPos: (" << potentialPos.x << "," << potentialPos.y << ")" << std::endl;


        HandleCollisions(deltaTime); // This should set m_IsOnGround if applicable

        if (shouldLogThisFrame) std::cout << "  [PCC::Update] After HandleCollisions, m_IsOnGround: " << (m_IsOnGround ? "T" : "F") << ", VVel: " << m_VerticalVelocity << std::endl;


        EnsureStateMachine();
        if (m_pStateMachine)
        {
            if (m_IsOnGround)
            {
                if (std::abs(m_Direction.x) > 0.01f)
                {
                    if (!m_pStateMachine->GetCurrentState() || typeid(*m_pStateMachine->GetCurrentState()) != typeid(PlayerRunState))
                    {
                        if (shouldLogThisFrame) std::cout << "  [PCC::Update] StateChange: Grounded & Moving -> PlayerRunState" << std::endl;
                        m_pStateMachine->ChangeState(std::make_unique<PlayerRunState>());
                    }
                }
                else
                {
                    if (!m_pStateMachine->GetCurrentState() || typeid(*m_pStateMachine->GetCurrentState()) != typeid(PlayerIdleState))
                    {
                        if (shouldLogThisFrame) std::cout << "  [PCC::Update] StateChange: Grounded & Still -> PlayerIdleState" << std::endl;
                        m_pStateMachine->ChangeState(std::make_unique<PlayerIdleState>());
                    }
                }
            }
            else // In Air
            {
                if (shouldLogThisFrame)
                {
                    std::cout << "  [PCC::Update] In Air. Current State: "
                        << (m_pStateMachine->GetCurrentState() ? typeid(*m_pStateMachine->GetCurrentState()).name() : "None")
                        << std::endl;
                    // Potentially change to PlayerFallState or PlayerJumpState here if you have them
                }
            }
        }
        if (shouldLogThisFrame) std::cout << "[PCC::Update END]" << std::endl;
        updateLogCounter++;
    }

    void PlayerCharacterComponent::HandleCollisions(float /*deltaTime*/)
    {
        if (!GetOwner() || !m_pPlayerCollider || !m_pTransform) return;

        static int collisionLogCounter = 0;
        const int logFrequency = 30; // Log roughly every half-second
        bool shouldLogThisFrame = (collisionLogCounter % logFrequency == 0);

        // === CRITICAL SECTION: SCENE AND OBJECT ACCESS ===
        dae::Scene* currentScene = nullptr;
        // How to get the current scene in 'dae'?
        // Example 1: currentScene = dae::SceneManager::GetInstance().GetActiveScene();
        // Example 2: currentScene = GetOwner()->GetScene();
        // CONSULT YOUR DAE ENGINE DOCUMENTATION/EXAMPLES FOR THIS.

        if (!currentScene)
        {
            if (shouldLogThisFrame) std::cout << "[PCC::HandleCollisions] No scene access, using fallback floor." << std::endl;

            // Fallback: if no scene access, use a simple "absolute floor"
            // Note: m_IsOnGround was set to false at the start of Update.
            // This logic will set it to true if the condition is met.
            glm::vec2 currentPosAfterTentativeMove = m_pTransform->GetPosition(); // Position after Update's tentative move
            if (shouldLogThisFrame)
            {
                std::cout << "  [PCC::HandleCollisions] Fallback check: Current Y=" << currentPosAfterTentativeMove.y
                    << ", Floor Y=" << 676.0f << ", IsOnGround (before this check): " << (m_IsOnGround ? "T" : "F") // Will be F due to reset in Update
                    << ", VVel: " << m_VerticalVelocity << std::endl;
            }

            if (currentPosAfterTentativeMove.y >= 676.0f)
            {
                if (shouldLogThisFrame) std::cout << "    [PCC::HandleCollisions] Player hit fallback floor at Y=" << currentPosAfterTentativeMove.y << ". Grounding player." << std::endl;
                glm::vec2 finalPos = currentPosAfterTentativeMove;
                finalPos.y = 676.0f;
                m_VerticalVelocity = 0.0f;
                m_IsOnGround = true; // Player becomes grounded here by fallback
                m_pTransform->SetPosition(finalPos.x, finalPos.y);
                if (m_pPlayerCollider) m_pPlayerCollider->SetPosition(finalPos.x, finalPos.y);
            }
            else
            {
                if (shouldLogThisFrame) std::cout << "    [PCC::HandleCollisions] Player above fallback floor. Y=" << currentPosAfterTentativeMove.y << std::endl;
                // m_IsOnGround remains false if this path is taken (unless set true by actual tile collision later)
            }
            collisionLogCounter++;
            return;
        }

        // ... (Your commented-out actual collision loop with tiles would go here) ...
        /*
        YOUR_ENGINE_SPECIFIC_COLLECTION_TYPE gameObjects; // = currentScene->YOUR_METHOD_TO_GET_OBJECTS();
        // ... loop ...
        */
        // === END CRITICAL SECTION ===
        collisionLogCounter++;
    }

    void PlayerCharacterComponent::Render() const
    {
        // Empty
    }

    void PlayerCharacterComponent::Move(float x, float /*y*/)
    {
        std::cout << "[PCC::Move] Called with x: " << x << ". Prev m_Direction.x: " << m_Direction.x;
        m_Direction.x += x;
        m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
        std::cout << " -> New m_Direction.x: " << m_Direction.x << std::endl;
    }

    void PlayerCharacterComponent::StopMove(float x, float /*y*/)
    {
        std::cout << "[PCC::StopMove] Called with x: " << x << ". Prev m_Direction.x: " << m_Direction.x;
        m_Direction.x -= x;
        m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
        std::cout << " -> New m_Direction.x: " << m_Direction.x << std::endl;
    }

    void PlayerCharacterComponent::Jump()
    {
        std::cout << "[PCC::Jump] Called. m_IsOnGround (at call time): " << (m_IsOnGround ? "true" : "false")
            << ", Current VVel: " << m_VerticalVelocity << std::endl;
        if (m_IsOnGround) // Check m_IsOnGround status *after* HandleCollisions in the previous frame's Update
        {
            m_VerticalVelocity = -m_JumpStrength;
            m_IsOnGround = false; // Will be set to false again at start of next Update anyway, but good for clarity here
            if (m_pSoundService)
            {
                // Example: m_pSoundService->Play("JumpSoundID", 0.5f); 
            }
            std::cout << "  [PCC::Jump] JUMP ACTION! New VVel: " << m_VerticalVelocity
                << ", m_IsOnGround set to false." << std::endl;
        }
        else
        {
            std::cout << "  [PCC::Jump] Jump failed: Player not on ground." << std::endl;
        }
    }

    void PlayerCharacterComponent::DoDamage(int amount)
    {
        std::cout << "[PCC::DoDamage] Called with amount: " << amount << std::endl;
        auto healthComponent = GetOwner()->GetComponent<HealthComponent>();
        if (healthComponent)
        {
            healthComponent->TakeDamage(amount);
            if (m_pSoundService)
            {
                m_pSoundService->LoadSound("Sound/KillEnemy.wav");
                m_pSoundService->OutputSound("Sound/KillEnemy.wav", 64);
            }
        }
    }

    void PlayerCharacterComponent::AddScore(int points)
    {
        std::cout << "[PCC::AddScore] Called with points: " << points << std::endl;
        auto scoreComponent = GetOwner()->GetComponent<ScoreComponent>();
        if (scoreComponent)
        {
            scoreComponent->AddScore(points);
            if (m_pSoundService)
            {
                m_pSoundService->LoadSound("Sound/GetFruit.wav");
                m_pSoundService->OutputSound("Sound/GetFruit.wav", 64);
            }
        }
    }

    void PlayerCharacterComponent::EnsureStateMachine()
    {
        if (!m_pStateMachine)
        {
            static bool ensureSmFirstCall = true;
            if (ensureSmFirstCall) std::cout << "[PCC::EnsureStateMachine] Attempting to get StateMachineComponent." << std::endl;

            auto sm = GetOwner()->GetComponent<StateMachineComponent>();
            if (sm)
            {
                if (ensureSmFirstCall) std::cout << "  [PCC::EnsureStateMachine] StateMachineComponent found." << std::endl;
                m_pStateMachine = sm.get();
                if (m_pStateMachine && !m_pStateMachine->GetCurrentState() && m_IsOnGround)
                {
                    if (ensureSmFirstCall) std::cout << "  [PCC::EnsureStateMachine] Initializing state to PlayerIdleState (Grounded)." << std::endl;
                    m_pStateMachine->ChangeState(std::make_unique<PlayerIdleState>());
                }
            }
            else
            {
                if (ensureSmFirstCall) std::cerr << "  [PCC::EnsureStateMachine] StateMachineComponent NOT found." << std::endl;
            }
            ensureSmFirstCall = false;
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