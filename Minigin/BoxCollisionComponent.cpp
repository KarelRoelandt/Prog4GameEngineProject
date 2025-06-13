#include <cmath> // For std::abs
#include "BoxCollisionComponent.h"

namespace dae
{
	// Constructor
	BoxCollisionComponent::BoxCollisionComponent(GameObject* owner, float x, float y, float width, float height, ColliderTag tag)
		: BaseComponent(owner),
		boundingBox{ x, y, width, height },
		tag(tag)
	{
	}

	// Destructor
	BoxCollisionComponent::~BoxCollisionComponent()
	{
	}

	void BoxCollisionComponent::Update(float /*deltaTime*/)
	{
	}

	void BoxCollisionComponent::Render() const
	{
	}

	// MODIFIED IsColliding Function
	// The second parameter is now the vertical displacement of 'this' object for the current frame.
	// For the player, this would be playerVerticalVelocity * deltaTime.
	bool BoxCollisionComponent::IsColliding(const BoxCollisionComponent& other, float verticalDisplacementOfThis) const
	{
		const AABB& boxA = this->boundingBox;    // 'this' object (e.g., player)
		const AABB& boxB = other.GetBoundingBox(); // 'other' object (e.g., tile)
		ColliderTag tagA = this->tag;
		ColliderTag tagB = other.GetTag();

		// --- Rule for PLAYER (A) vs. SMALL_TILE (B) ---
		// This is a specific top-only collision rule with swept logic.
		if (tagA == ColliderTag::PLAYER && tagB == ColliderTag::SMALL_TILE)
		{
			// First, check for horizontal overlap. If none, can't land on top.
			bool horizontalOverlap = boxA.GetLeft() < boxB.GetRight() && boxA.GetRight() > boxB.GetLeft();
			if (!horizontalOverlap)
			{
				return false;
			}

			// Case 1: Player is moving downwards (verticalDisplacementOfThis > 0) - Potential landing
			if (verticalDisplacementOfThis > 0.001f) // Use a small threshold to consider it "moving significantly"
			{
				AABB playerBoxAtFrameStart = boxA;
				// Calculate player's Y position at the start of this frame's vertical movement
				playerBoxAtFrameStart.y -= verticalDisplacementOfThis;

				// Condition A: Player's bottom was at or above the tile's top at the frame's start.
				// (Epsilon for floating point comparisons)
				bool prevBottomWasAtOrAboveTileTop = playerBoxAtFrameStart.GetBottom() <= boxB.GetTop() + 0.1f;

				// Condition B: Player's bottom is now at or below the tile's top at the frame's end.
				bool currentBottomIsAtOrBelowTileTop = boxA.GetBottom() >= boxB.GetTop() - 0.1f;

				// Condition C: Player's top is above the tile's top (ensuring we are not coming from below and passing through)
				bool currentTopIsAboveTileTop = boxA.GetTop() < boxB.GetTop();


				if (prevBottomWasAtOrAboveTileTop && currentBottomIsAtOrBelowTileTop && currentTopIsAboveTileTop)
				{
					return true; // Successful landing on top of SMALL_TILE
				}
			}
			// Case 2: Player is stationary or moving upwards (verticalDisplacementOfThis <= 0) - Check for resting on top
			else
			{
				float onTopEpsilon = 0.5f; // Tolerance for being "on top" when resting

				// Condition A: Player's bottom is very close to the tile's top surface.
				bool bottomIsOnTileTop = (boxA.GetBottom() >= boxB.GetTop() - onTopEpsilon) &&
					(boxA.GetBottom() <= boxB.GetTop() + onTopEpsilon);

				// Condition B: Player's top is above or very near the tile's top (not under it).
				// This ensures we don't register a collision if the player is somehow under the tile but their bottom edge aligns.
				bool topIsNearOrAboveTileTop = boxA.GetTop() < boxB.GetTop() + onTopEpsilon;


				if (bottomIsOnTileTop && topIsNearOrAboveTileTop)
				{
					return true; // Considered resting on ground (SMALL_TILE)
				}
			}

			// If neither landing nor resting conditions for PLAYER vs SMALL_TILE are met,
			// then it's not a valid "top-only" collision for this pair.
			return false;
		}

		// --- Rule for TILE (A) vs. PLAYER (B) ---
		// This case is when a tile (this) is checking against a player (other).
		// verticalDisplacementOfThis would be for the tile, likely 0 if static.
		// This should be a simple overlap. The player's own IsColliding call (where player is 'this')
		// is responsible for the detailed top-only swept logic.
		if (tagA == ColliderTag::SMALL_TILE && tagB == ColliderTag::PLAYER)
		{
			bool hOverlap = boxA.GetLeft() < boxB.GetRight() && boxA.GetRight() > boxB.GetLeft();
			bool vOverlap = boxA.GetTop() < boxB.GetBottom() && boxA.GetBottom() > boxB.GetTop();
			return hOverlap && vOverlap;
		}

		// --- General Overlap Check for all other cases ---
		// This includes:
		// - PLAYER (A) vs. BIG_TILE (B) (and vice-versa)
		// - PLAYER (A) vs. GENERIC (B) (and vice-versa)
		// - TILE (A) vs. GENERIC (B) (and vice-versa)
		// - GENERIC (A) vs. GENERIC (B)
		// For these, any standard AABB overlap counts as a collision.
		bool generalHorizontalOverlap = boxA.GetLeft() < boxB.GetRight() && boxA.GetRight() > boxB.GetLeft();
		bool generalVerticalOverlap = boxA.GetTop() < boxB.GetBottom() && boxA.GetBottom() > boxB.GetTop();

		if (generalHorizontalOverlap && generalVerticalOverlap)
		{
			return true;
		}

		return false; // No collision by any rule
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