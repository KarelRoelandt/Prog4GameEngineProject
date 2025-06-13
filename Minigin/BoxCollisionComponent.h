#ifndef BOX_COLLISION_COMPONENT_H
#define BOX_COLLISION_COMPONENT_H
#include "BaseComponent.h"

namespace dae
{

	// Forward declaration if you have a base Component class
	// class Component; 

	// Assuming a simple structure for a 2D bounding box
	struct AABB
	{
	    float x;
	    float y;
	    float width;
	    float height;

	    // Helper to get edge coordinates
	    float GetTop() const { return y; }
	    float GetBottom() const { return y + height; }
	    float GetLeft() const { return x; }
	    float GetRight() const { return x + width; }
	};

	// Enum to identify the type of collider
	enum class ColliderTag
	{
	    PLAYER,
	    SMALL_TILE,
	    BIG_TILE,   // New tag for big tiles
	    GENERIC     // Default for other collidable objects
	    // Add other tags as needed (e.g., ENEMY, WALL)
	};


	// If you have a TransformComponent, you might want to include its header
	// #include "TransformComponent.h" 

	class BoxCollisionComponent final : public BaseComponent
	{
	public:
		BoxCollisionComponent(GameObject* owner, float x, float y, float width, float height, ColliderTag tag = ColliderTag::GENERIC);
		~BoxCollisionComponent();
		void Update(float);
		void Render() const;

		// Updates the bounding box, potentially based on a TransformComponent
		// void Update(float deltaTime, const TransformComponent& transform); 

		// Checks collision with another BoxCollisionComponent.
		// playerVerticalVelocity is a hint for player's movement direction for specific rules.
		bool IsColliding(const BoxCollisionComponent& other, float playerVerticalVelocity = 0.0f) const;

		// Getters
		const AABB& GetBoundingBox() const;
		ColliderTag GetTag() const;

		// Setters
		void SetPosition(float x, float y); // Might be better to update via TransformComponent
		void SetDimensions(float width, float height);
		void SetTag(ColliderTag newTag);

	private:
		AABB boundingBox;
		ColliderTag tag;
		// Potentially a pointer or reference to a TransformComponent
		// TransformComponent* ownerTransform; 
	};

}

#endif // BOX_COLLISION_COMPONENT_H