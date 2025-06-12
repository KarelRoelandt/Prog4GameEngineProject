#include "TransformComponent.h"
#include "GameObject.h" // Include GameObject.h here to break the circular dependency

namespace dae
{
    void TransformComponent::UpdateWorldTransform()
    {
        if (auto owner = GetOwner())
        {
            GameObject* parent = owner->GetParent();
            if (parent)
            {
                // Get the parent's transform component and its world transform
                auto parentTransformComp = parent->GetComponent<TransformComponent>();
                if (parentTransformComp)
                {
                    const glm::mat4& parentTransform = parentTransformComp->GetWorldTransform();

                    // Create the local transform matrix
                    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), m_LocalPosition) *
                        glm::mat4_cast(m_LocalRotation) *
                        glm::scale(glm::mat4(1.0f), m_LocalScale);

                    // Combine with parent transform
                    m_WorldTransform = parentTransform * localTransform;
                }
                else
                {
                    // Parent has no transform, just use local transforms
                    m_WorldTransform = glm::translate(glm::mat4(1.0f), m_LocalPosition) *
                        glm::mat4_cast(m_LocalRotation) *
                        glm::scale(glm::mat4(1.0f), m_LocalScale);
                }
            }
            else
            {
                // No parent, just use local transforms
                m_WorldTransform = glm::translate(glm::mat4(1.0f), m_LocalPosition) *
                    glm::mat4_cast(m_LocalRotation) *
                    glm::scale(glm::mat4(1.0f), m_LocalScale);
            }
        }
        m_IsDirty = false;
    }
}