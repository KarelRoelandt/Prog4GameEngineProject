#include "TransformComponent.h"
#include "GameObject.h" // Required for GetOwner, GetParent, GetChildren, GetComponent

namespace dae
{
    TransformComponent::TransformComponent(GameObject* owner)
        : BaseComponent(owner),
        m_LocalPosition(0.0f, 0.0f, 0.0f),
        m_LocalRotation(glm::quat()), // Identity quaternion
        m_LocalScale(1.0f, 1.0f, 1.0f),
        m_WorldTransform(1.0f),       // Identity matrix
        m_IsDirty(true)               // Start dirty, forces first calculation
    {
    }

    void TransformComponent::Update(float /*deltaTime*/)
    {
        // With a fully lazy system + recursive SetDirty, this can be optional.
        // If GetWorldTransform() is always called before use, this explicit update might not be needed.
        // However, keeping it ensures that if the game loop has an explicit "update transforms" phase,
        // it works as expected.
        if (m_IsDirty)
        {
            UpdateWorldTransform();
        }
    }

    void TransformComponent::Render() const
    {
        // TransformComponent typically does not render anything itself.
    }

    void TransformComponent::SetLocalPosition(const glm::vec3& position)
    {
        if (m_LocalPosition != position)
        {
            m_LocalPosition = position;
            this->SetDirty();
        }
    }

    const glm::vec3& TransformComponent::GetLocalPosition() const
    {
        return m_LocalPosition;
    }

    void TransformComponent::SetLocalRotation(const glm::quat& rotation)
    {
        // Comparing quaternions for exact equality can be tricky due to float precision
        // and q == -q representing the same rotation. A small epsilon comparison might be better
        // if optimization is critical, but for now, direct comparison is simpler.
        if (m_LocalRotation != rotation)
        {
            m_LocalRotation = rotation;
            this->SetDirty();
        }
    }

    const glm::quat& TransformComponent::GetLocalRotation() const
    {
        return m_LocalRotation;
    }

    void TransformComponent::SetLocalScale(const glm::vec3& scale)
    {
        if (m_LocalScale != scale)
        {
            m_LocalScale = scale;
            this->SetDirty();
        }
    }

    const glm::vec3& TransformComponent::GetLocalScale() const
    {
        return m_LocalScale;
    }

    const glm::mat4& TransformComponent::GetWorldTransform() const
    {
        if (m_IsDirty)
        {
            // const_cast is used here to call a non-const method from a const one
            // for lazy evaluation. An alternative is to mark m_WorldTransform and m_IsDirty
            // as 'mutable' and make UpdateWorldTransform() const.
            const_cast<TransformComponent*>(this)->UpdateWorldTransform();
        }
        return m_WorldTransform;
    }

    void TransformComponent::SetPosition(float x, float y)
    {
        if (m_LocalPosition.x != x || m_LocalPosition.y != y)
        {
            m_LocalPosition.x = x;
            m_LocalPosition.y = y;
            // m_LocalPosition.z remains unchanged
            this->SetDirty();
        }
    }

    glm::vec2 TransformComponent::GetPosition() const
    {
        return glm::vec2(m_LocalPosition.x, m_LocalPosition.y);
    }

    void TransformComponent::SetDirty()
    {
        m_IsDirty = true;

        // When this transform becomes dirty, all its children's world transforms
        // also become invalid, so they must be marked dirty as well.
        if (auto owner = GetOwner())
        {
            for (auto* childGameObject : owner->GetChildren())
            {
                if (childGameObject)
                {
                    // Assuming GameObject::GetTransform() exists and returns TransformComponent*
                    // or shared_ptr<TransformComponent>
                    if (auto childTransformComp = childGameObject->GetTransform())
                    {
                        childTransformComp->SetDirty(); // Recursive call
                    }
                }
            }
        }
    }

    void TransformComponent::PrintPositions() const
    {
        std::cout << "Local Position: (" << m_LocalPosition.x << ", " << m_LocalPosition.y << ", " << m_LocalPosition.z << ")\n";
        const glm::mat4& worldTransformMat = GetWorldTransform(); // Ensures it's up-to-date
        glm::vec3 worldPos = glm::vec3(worldTransformMat[3]);
        std::cout << "World Position: (" << worldPos.x << ", " << worldPos.y << ", " << worldPos.z << ")\n";
    }

    void TransformComponent::UpdateWorldTransform()
    {
        glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), m_LocalPosition) *
            glm::mat4_cast(m_LocalRotation) *
            glm::scale(glm::mat4(1.0f), m_LocalScale);

        if (auto owner = GetOwner())
        {
            if (auto parent = owner->GetParent())
            {
                if (auto parentTransformComp = parent->GetTransform()) // Use GameObject::GetTransform()
                {
                    const glm::mat4& parentWorldTransform = parentTransformComp->GetWorldTransform();
                    m_WorldTransform = parentWorldTransform * localTransform;
                }
                else
                {
                    // Parent exists but has no TransformComponent (should be rare)
                    m_WorldTransform = localTransform;
                }
            }
            else
            {
                // No parent, world transform is the local transform
                m_WorldTransform = localTransform;
            }
        }
        else
        {
            // No owner, should not typically happen for a component in use.
            // Default to local transform or identity if m_Local members are not initialized.
            m_WorldTransform = localTransform;
        }

        m_IsDirty = false;
        // Children are already marked dirty by the recursive SetDirty() calls
        // originating from local setters or direct calls to SetDirty() on this or an ancestor.
    }
}