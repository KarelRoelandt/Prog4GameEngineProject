#include "ZenChanMovementComponent.h"
#include "Scene.h"
#include "GameObject.h"
#include <iostream>

namespace dae
{

    void ZenChanMovementComponent::Update(float deltaTime)
    {
        // Initialize component references if not done yet
        if (!m_pTransform)
        {
            auto transformPtr = GetOwner()->GetComponent<TransformComponent>();
            if (transformPtr)
            {
                m_pTransform = transformPtr.get();
                m_InitialX = m_pTransform->GetPosition().x;
            }
            else
            {
                std::cout << "[ZenChan::Update] Owner missing TransformComponent." << "\n";
                return;
            }
        }

        if (!m_pCollisionComponent)
        {
            auto colliderPtr = GetOwner()->GetComponent<BoxCollisionComponent>();
            if (colliderPtr) m_pCollisionComponent = colliderPtr.get();
            else
            {
                std::cout << "[ZenChan::Update] Owner missing BoxCollisionComponent." << "\n";
                return;
            }
        }

        if (!m_pAnimator)
        {
            auto animatorPtr = GetOwner()->GetComponent<AnimatorComponent>();
            if (animatorPtr) m_pAnimator = animatorPtr.get();
            // Not critical if missing
        }

        m_ElapsedTime += deltaTime;

        // Change direction periodically
        if (m_ElapsedTime > m_DirectionChangeInterval)
        {
            m_MovementDirection *= -1.0f;
            m_ElapsedTime = 0.0f;
        }

        // Check if we've moved beyond patrol range
        glm::vec2 currentPos = m_pTransform->GetPosition();
        if (std::abs(currentPos.x - m_InitialX) > m_PatrolRange)
        {
            m_MovementDirection *= -1.0f; // Reverse direction
        }

        // First update horizontal movement
        UpdateMovement(deltaTime);

        // IMPORTANT: Check if there's ground beneath us AFTER moving horizontally
        CheckGroundBeneath();

        // Then apply gravity 
        ApplyGravity(deltaTime);

        // Finally handle collisions
        HandleCollisions();

        // Update animation direction
        if (m_pAnimator)
        {
            if (m_MovementDirection < 0)
            {
                m_pAnimator->SetFlip(true);  // Face left
            }
            else
            {
                m_pAnimator->SetFlip(false); // Face right
            }
        }
    }

    void ZenChanMovementComponent::ApplyGravity(float deltaTime)
    {
        if (!m_IsOnGround)
        {
            m_VerticalVelocity += m_Gravity * deltaTime;

            // Clamp fall speed
            if (m_VerticalVelocity > m_MaxFallSpeed)
            {
                m_VerticalVelocity = m_MaxFallSpeed;
            }
        }
        else
        {
            m_VerticalVelocity = 0.0f;
        }

        // Apply vertical movement
        if (m_pTransform)
        {
            glm::vec2 pos = m_pTransform->GetPosition();
            pos.y += m_VerticalVelocity * deltaTime;
            m_pTransform->SetPosition(pos.x, pos.y);

            // Update collision box position
            if (m_pCollisionComponent)
            {
                m_pCollisionComponent->SetPosition(pos.x, pos.y);
            }
        }

        if (!m_IsOnGround)
        {
            m_CurrentState = MovementState::FALLING;
        }
    }

    // NEW METHOD: Check if there's ground directly beneath ZenChan
    void ZenChanMovementComponent::CheckGroundBeneath()
    {
        if (!m_pCollisionComponent || !m_pTransform) return;

        auto scene = GetOwner()->GetScene();
        if (!scene) return;

        // Save the current on-ground state
        bool wasOnGround = m_IsOnGround;

        // Temporarily assume we're not on ground until proven otherwise
        m_IsOnGround = false;

        // Get ZenChan's current bounding box
        AABB zenChanAABB = m_pCollisionComponent->GetBoundingBox();

        // Check for hard floor first (optimization)
        float hardFloorY = 724.0f - zenChanAABB.height;
        if (zenChanAABB.GetBottom() >= hardFloorY - 1.0f)
        {
            m_IsOnGround = true;
            return;
        }

        // Create a small sensor box just below ZenChan's feet to check for ground
        AABB groundSensor;
        groundSensor.x = zenChanAABB.x;
        groundSensor.y = zenChanAABB.GetBottom() + 1.0f; // Just below feet
        groundSensor.width = zenChanAABB.width;
        groundSensor.height = 2.0f; // Small height for the sensor

        const auto& gameObjects = scene->GetAllGameObjects();

        for (const auto& otherGameObject : gameObjects)
        {
            // Skip self
            if (otherGameObject.get() == GetOwner()) continue;

            auto otherColliderComponent = otherGameObject->GetComponent<BoxCollisionComponent>();
            if (!otherColliderComponent) continue;

            ColliderTag otherTag = otherColliderComponent->GetTag();

            // Only check tiles for ground
            if (otherTag != ColliderTag::SMALL_TILE && otherTag != ColliderTag::BIG_TILE) continue;

            AABB otherAABB = otherColliderComponent->GetBoundingBox();

            // Check if our ground sensor overlaps with this tile
            bool horizontalOverlap = groundSensor.GetLeft() < otherAABB.GetRight() &&
                groundSensor.GetRight() > otherAABB.GetLeft();

            bool verticalOverlap = groundSensor.GetTop() < otherAABB.GetBottom() &&
                groundSensor.GetBottom() > otherAABB.GetTop();

            if (horizontalOverlap && verticalOverlap)
            {
                // Check if the top of the tile is close to ZenChan's feet
                float distanceToTileTop = std::abs(zenChanAABB.GetBottom() - otherAABB.GetTop());
                if (distanceToTileTop <= 0.5f)
                {
                    m_IsOnGround = true;
                    break;
                }
            }
        }

        // If we were on ground and now we're not, we just walked off an edge
        if (wasOnGround && !m_IsOnGround)
        {
            m_CurrentState = MovementState::FALLING;
            m_VerticalVelocity = 0.01f; // Small initial velocity to start falling
        }
    }

    void ZenChanMovementComponent::HandleCollisions()
    {
        if (!m_pCollisionComponent || !m_pTransform) return;

        auto scene = GetOwner()->GetScene();
        if (!scene)
        {
            // Fallback ground collision if no scene
            glm::vec2 fallbackPos = m_pTransform->GetPosition();
            AABB zenChanBox = m_pCollisionComponent->GetBoundingBox();
            float groundY = 724.0f - zenChanBox.height;  // Match the hardcoded floor from PlayerCharacterComponent
            if (fallbackPos.y >= groundY && m_VerticalVelocity >= 0)
            {
                fallbackPos.y = groundY;
                m_pTransform->SetPosition(fallbackPos.x, fallbackPos.y);
                m_pCollisionComponent->SetPosition(fallbackPos.x, fallbackPos.y);
                m_VerticalVelocity = 0.0f;
                m_IsOnGround = true;
            }
            return;
        }

        const auto& gameObjects = scene->GetAllGameObjects();
        bool landedThisFrame = false;

        for (const auto& otherGameObject : gameObjects)
        {
            // Skip self
            if (otherGameObject.get() == GetOwner()) continue;

            auto otherColliderComponent = otherGameObject->GetComponent<BoxCollisionComponent>();
            if (!otherColliderComponent) continue;

            BoxCollisionComponent* otherColliderRawPtr = otherColliderComponent.get();
            AABB otherAABB = otherColliderRawPtr->GetBoundingBox();
            ColliderTag otherTag = otherColliderRawPtr->GetTag();

            AABB zenChanAABB = m_pCollisionComponent->GetBoundingBox();
            glm::vec2 currentPosition = m_pTransform->GetPosition();

            // Check for collision using the swept collision detection
            if (m_pCollisionComponent->IsColliding(*otherColliderRawPtr, m_VerticalVelocity))
            {
                glm::vec2 resolvedPosition = currentPosition;
                bool collisionResolvedThisIteration = false;

                // Handle BIG_TILE collisions
                if (otherTag == ColliderTag::BIG_TILE)
                {
                    glm::vec2 zenChanCenter = { zenChanAABB.GetLeft() + zenChanAABB.width / 2.0f, zenChanAABB.GetTop() + zenChanAABB.height / 2.0f };
                    glm::vec2 tileCenter = { otherAABB.GetLeft() + otherAABB.width / 2.0f, otherAABB.GetTop() + otherAABB.height / 2.0f };
                    float deltaX = zenChanCenter.x - tileCenter.x;
                    float deltaY = zenChanCenter.y - tileCenter.y;
                    float combinedHalfWidths = zenChanAABB.width / 2.0f + otherAABB.width / 2.0f;
                    float combinedHalfHeights = zenChanAABB.height / 2.0f + otherAABB.height / 2.0f;
                    float overlapX = combinedHalfWidths - std::abs(deltaX);
                    float overlapY = combinedHalfHeights - std::abs(deltaY);

                    bool resolvedHorizontally = false;
                    if (overlapX > 0.001f && (overlapX < overlapY || overlapY <= 0.001f))
                    {
                        // Resolve horizontal collision (wall)
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

                        // Change direction when hitting a wall
                        m_MovementDirection *= -1.0f;
                    }

                    float epsilonResting = 0.5f;
                    bool restingOnBigTileTop = (!resolvedHorizontally && m_VerticalVelocity <= 0.f &&
                        (zenChanAABB.GetBottom() >= otherAABB.GetTop() - epsilonResting && zenChanAABB.GetBottom() <= otherAABB.GetTop() + epsilonResting) &&
                        (zenChanAABB.GetLeft() < otherAABB.GetRight() - epsilonResting && zenChanAABB.GetRight() > otherAABB.GetLeft() + epsilonResting) &&
                        zenChanAABB.GetTop() < otherAABB.GetTop());

                    if ((overlapY > 0.001f && (!resolvedHorizontally || overlapY < overlapX)) || restingOnBigTileTop)
                    {
                        if (deltaY > 0 && !restingOnBigTileTop)
                        {
                            resolvedPosition.y = currentPosition.y + overlapY;
                            if (m_VerticalVelocity < 0) m_VerticalVelocity = 0;
                        }
                        else
                        {
                            resolvedPosition.y = otherAABB.GetTop() - zenChanAABB.height;
                            if (m_VerticalVelocity >= 0)
                            {
                                m_VerticalVelocity = 0.0f;
                                landedThisFrame = true;
                            }
                        }
                        collisionResolvedThisIteration = true;
                    }
                }
                // Handle SMALL_TILE collisions (platform)
                else if (otherTag == ColliderTag::SMALL_TILE)
                {
                    bool landedOnSmallTileThisIteration = false;
                    glm::vec2 smallTileResolvedPosition = currentPosition;
                    float onTopEpsilon = 0.5f;

                    bool horizontalOverlap = (zenChanAABB.GetLeft() < otherAABB.GetRight() - onTopEpsilon &&
                        zenChanAABB.GetRight() > otherAABB.GetLeft() + onTopEpsilon);

                    if (horizontalOverlap)
                    {
                        if (m_VerticalVelocity > 0.f)
                        {
                            // ZenChan is falling
                            if (zenChanAABB.GetTop() < otherAABB.GetTop() &&
                                zenChanAABB.GetBottom() >= otherAABB.GetTop() - onTopEpsilon)
                            {
                                smallTileResolvedPosition.y = otherAABB.GetTop() - zenChanAABB.height;
                                m_VerticalVelocity = 0.0f;
                                landedOnSmallTileThisIteration = true;
                            }
                        }
                        else
                        {
                            // ZenChan is stationary or moving up
                            bool feetAreCorrectlyPositioned = (zenChanAABB.GetBottom() >= otherAABB.GetTop() - onTopEpsilon &&
                                zenChanAABB.GetBottom() <= otherAABB.GetTop() + onTopEpsilon);
                            bool zenChanIsAboveTile = zenChanAABB.GetTop() < otherAABB.GetTop();

                            if (feetAreCorrectlyPositioned && zenChanIsAboveTile)
                            {
                                smallTileResolvedPosition.y = otherAABB.GetTop() - zenChanAABB.height;
                                m_VerticalVelocity = 0.0f;
                                landedOnSmallTileThisIteration = true;
                            }
                        }
                    }

                    if (landedOnSmallTileThisIteration)
                    {
                        landedThisFrame = true;
                        resolvedPosition.y = smallTileResolvedPosition.y;
                        resolvedPosition.x = currentPosition.x; // Only Y is resolved
                        collisionResolvedThisIteration = true;
                    }
                }

                if (collisionResolvedThisIteration)
                {
                    m_pTransform->SetPosition(resolvedPosition.x, resolvedPosition.y);
                    if (m_pCollisionComponent)
                    {
                        m_pCollisionComponent->SetPosition(resolvedPosition.x, resolvedPosition.y);
                    }
                }
            }
        }

        if (landedThisFrame)
        {
            m_IsOnGround = true;
        }

        // Hard floor collision as fallback (match the player's hardcoded floor)
        AABB zenChanBoxForHardFloor = m_pCollisionComponent->GetBoundingBox();
        float hardFloorY = 724.0f - zenChanBoxForHardFloor.height;

        glm::vec2 finalPosition = m_pTransform->GetPosition();
        if (finalPosition.y > hardFloorY)
        {
            finalPosition.y = hardFloorY;
            m_pTransform->SetPosition(finalPosition.x, finalPosition.y);
            if (m_pCollisionComponent)
            {
                m_pCollisionComponent->SetPosition(finalPosition.x, finalPosition.y);
            }
            if (m_VerticalVelocity > 0)
            {
                m_VerticalVelocity = 0.0f;
            }
            m_IsOnGround = true;
        }

        // If we landed this frame, update state
        if (m_IsOnGround && m_CurrentState == MovementState::FALLING)
        {
            m_CurrentState = (m_MovementDirection < 0) ?
                MovementState::WALKING_LEFT :
                MovementState::WALKING_RIGHT;
        }
    }

    void ZenChanMovementComponent::UpdateMovement(float deltaTime)
    {
        // Allow movement even while falling

        if (m_pTransform)
        {
            glm::vec2 pos = m_pTransform->GetPosition();
            float horizontalMovement = m_MovementSpeed * m_MovementDirection * deltaTime;
            pos.x += horizontalMovement;
            m_pTransform->SetPosition(pos.x, pos.y);

            // Update collision box position
            if (m_pCollisionComponent)
            {
                m_pCollisionComponent->SetPosition(pos.x, pos.y);
            }

            // Only update state if on ground
            if (m_IsOnGround)
            {
                m_CurrentState = (m_MovementDirection < 0) ?
                    MovementState::WALKING_LEFT :
                    MovementState::WALKING_RIGHT;
            }
        }
    }

} // namespace dae