

#include <cmath> // For std::abs, useful for float comparisons with tolerance


#include "BoxCollisionComponent.h"



namespace dae
{
	// Constructor
	BoxCollisionComponent::BoxCollisionComponent(GameObject* owner, float x, float y, float width, float height, ColliderTag tag)
		: BaseComponent(owner), // Pass a valid GameObject* here if available
		boundingBox{ x, y, width, height },
		tag(tag)
	{
		// If linked to a TransformComponent, you'd get it from the parent entity/object here
	}

	// Destructor
	BoxCollisionComponent::~BoxCollisionComponent()
	{
	}

	 //void BoxCollisionComponent::Update(float deltaTime, const TransformComponent& transform) {
	 //    // Update boundingBox based on the transform's current state
	 //    // E.g., boundingBox.x = transform.GetPosition().x - boundingBox.width / 2.0f; (if origin is center)
	 //    //       boundingBox.y = transform.GetPosition().y - boundingBox.height / 2.0f;
	 //}

	void BoxCollisionComponent::Update(float /*deltaTime*/)
	{
		// Implement logic if needed, or leave empty if not used
	}

	void BoxCollisionComponent::Render() const
	{
		// Implement logic if needed, or leave empty if not used
	}

	bool BoxCollisionComponent::IsColliding(const BoxCollisionComponent& other, float playerVerticalVelocity) const
	{
		const AABB& boxA = this->boundingBox;       // The component doing the checking (e.g., player)
		const AABB& boxB = other.GetBoundingBox(); // The component being checked against (e.g., tile)
		ColliderTag tagA = this->tag;
		ColliderTag tagB = other.GetTag();

		// Basic AABB overlap check (both horizontal and vertical)
		bool horizontalOverlap = boxA.GetLeft() < boxB.GetRight() && boxA.GetRight() > boxB.GetLeft();
		bool verticalOverlap = boxA.GetTop() < boxB.GetBottom() && boxA.GetBottom() > boxB.GetTop();

		if (!horizontalOverlap || !verticalOverlap)
		{
			return false; // No overlap at all, definitely no collision
		}

		// --- Specific Collision Rules ---

		// Rule 1: Player (A) vs. Small Tile (B) - Top-only collision
		if (tagA == ColliderTag::PLAYER && tagB == ColliderTag::SMALL_TILE)
		{
			bool playerIsMovingDown = playerVerticalVelocity > 0.0f;
			float onTopTolerance = 2.0f;

			bool playerBottomNearTileTop = boxA.GetBottom() >= boxB.GetTop() &&
				boxA.GetBottom() <= (boxB.GetTop() + onTopTolerance);

			if (playerBottomNearTileTop && playerIsMovingDown)
			{
				if (boxA.GetTop() < boxB.GetBottom())
				{
					return true; // Collision with the top surface
				}
			}
			return false; // Not a "top collision" as defined for player on small tile
		}
		// Symmetrical check for Small Tile (A) vs. Player (B) - if tiles can initiate checks against player
		else if (tagA == ColliderTag::SMALL_TILE && tagB == ColliderTag::PLAYER)
		{
			// 'other' is the player in this case. We need its vertical velocity.
			// This example assumes playerVerticalVelocity is always for 'this' if 'this' is the player,
			// or for 'other' if 'other' is the player.
			// A more robust system might pass velocities for both objects or have a central physics system handle this.
			// For now, let's assume playerVerticalVelocity is relevant if one of the colliders is a player.
			// If 'other' is the player, its velocity would be `playerVerticalVelocity` passed to the function.
			// However, the parameter is named `playerVerticalVelocity` implying it's for the player.
			// Let's assume the call is always `player.IsColliding(tile, playerVel)`.
			// So, this symmetrical case might need careful handling of velocity.
			// For simplicity, we'll just return true for general overlap if tile checks player,
			// or rely on Player checking the tile using the rule above.
			// A truly symmetrical top-only check here would be:
			//   bool playerIsMovingDown = playerVerticalVelocity > 0.0f; // This is velocity of 'other' (player)
			//   float onTopTolerance = 2.0f;
			//   bool playerBottomNearTileTop = boxB.GetBottom() >= boxA.GetTop() &&
			//                                  boxB.GetBottom() <= (boxA.GetTop() + onTopTolerance);
			//   if (playerBottomNearTileTop && playerIsMovingDown) { ... }
			// Given the prompt, it's likely the player initiates the check.
			// If a small tile initiated, and it's a general overlap, we'd let the player's check determine the top-only.
			// So, if small_tile is 'A', we can treat it as a general collision here, and the player's check will refine.
			return true; // General overlap, Player's check will enforce top-only.
		}

		// Rule 2: Player (A) vs. Big Tile (B) - All-sides collision
		if ((tagA == ColliderTag::PLAYER && tagB == ColliderTag::BIG_TILE) ||
			(tagA == ColliderTag::BIG_TILE && tagB == ColliderTag::PLAYER))
		{
			return true; // Standard AABB collision (any overlap counts)
		}

		// Default: Standard AABB collision for all other tag combinations
		// This includes GENERIC vs GENERIC, PLAYER vs GENERIC, TILE vs GENERIC etc.
		return true; // General overlap was detected
	}

	const AABB& BoxCollisionComponent::GetBoundingBox() const
	{
		return boundingBox;
	}

	ColliderTag BoxCollisionComponent::GetTag() const
	{
		return tag;
	}

	void BoxCollisionComponent::SetPosition(float x, float y)
	{
		boundingBox.x = x;
		boundingBox.y = y;
	}

	void BoxCollisionComponent::SetDimensions(float width, float height)
	{
		boundingBox.width = width;
		boundingBox.height = height;
	}

	void BoxCollisionComponent::SetTag(ColliderTag newTag)
	{
		tag = newTag;
	}
}