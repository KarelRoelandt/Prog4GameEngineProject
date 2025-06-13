#include "GameObject.h"
// TransformComponent.h is already included via GameObject.h
#include <algorithm> // For std::remove
#include <iostream>  // For std::cout (though warnings are now prefixed from GameObject.h)

// GLM includes for matrix operations in SetParent
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

// Note: The warning prefix constants (e.g., dae::WARNING_PREFIX_GAMEOBJECT_H)
// are defined in GameObject.h to be accessible by the template AddComponent method.

namespace dae
{
    GameObject::GameObject()
        : m_Name("DefaultGameObject") // Initialize with a default name
    {
        // Automatically add a TransformComponent.
        // AddComponent method handles the logic to ensure only one is added.
        AddComponent<dae::TransformComponent>();
    }

    GameObject::~GameObject()
    {
        // When a GameObject is destroyed, it should unparent itself from its current parent.
        if (m_Parent)
        {
            // This is a simplified removal. A more robust way would be for m_Parent to have a RemoveChild method.
            auto& siblings = m_Parent->m_Children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }

        // Children are raw pointers. Their lifecycle must be managed externally (e.g., by a Scene).
        // If this GameObject "owned" its children (e.g., via std::vector<std::unique_ptr<GameObject>>),
        // they would be automatically destroyed here.
        // For now, we assume children are managed elsewhere or this is a flat list of non-owning pointers.
    }

    void GameObject::Update(float deltaTime)
    {
        // Update all components
        for (auto& componentPair : m_Components)
        {
            if (componentPair.second) // Ensure component pointer is valid
            {
                componentPair.second->Update(deltaTime);
            }
        }
    }

    void GameObject::Render() const
    {
        // Render all components
        for (const auto& componentPair : m_Components)
        {
            if (componentPair.second) // Ensure component pointer is valid
            {
                componentPair.second->Render();
            }
        }
    }

    void GameObject::SetName(const std::string& name)
    {
        m_Name = name;
    }

    const std::string& GameObject::GetName() const
    {
        return m_Name;
    }

    GameObject* GameObject::GetParent() const
    {
        return m_Parent;
    }

    const std::vector<GameObject*>& GameObject::GetChildren() const
    {
        return m_Children;
    }

    dae::TransformComponent* GameObject::GetTransform() const
    {
        return GetComponent<dae::TransformComponent>().get();
    }

    bool GameObject::IsDescendant(GameObject* potentialDescendant) const
    {
        if (potentialDescendant == nullptr) return false;

        for (const auto* child : m_Children)
        {
            if (child == nullptr) continue;
            if (child == potentialDescendant)
            {
                return true; // Direct child
            }
            // Recursively check in grandchildren
            if (child->IsDescendant(potentialDescendant))
            {
                return true;
            }
        }
        return false;
    }

    void GameObject::SetParent(GameObject* newParent, bool keepWorldPosition)
    {
        // Avoid re-parenting to the same parent or self
        if (m_Parent == newParent) return;
        if (newParent == this)
        {
            std::cout << dae::WARNING_PREFIX_GAMEOBJECT_H << "GameObject::SetParent - Cannot parent object '" << m_Name << "' to itself." << std::endl;
            return;
        }

        // Prevent circular dependencies (parenting to one of its own children)
        if (newParent != nullptr && this->IsDescendant(newParent))
        {
            std::cout << dae::WARNING_PREFIX_GAMEOBJECT_H << "GameObject::SetParent - Circular dependency detected. Cannot parent '"
                << m_Name << "' to one of its own descendants ('" << newParent->GetName() << "')." << std::endl;
            return;
        }

        dae::TransformComponent* currentObjectTransform = GetTransform(); // Should always exist

        // 1. Store old world transform if keepWorldPosition is true
        glm::mat4 oldWorldMatrix(1.0f); // Identity matrix
        if (keepWorldPosition && currentObjectTransform)
        {
            oldWorldMatrix = currentObjectTransform->GetWorldTransform();
        }

        // 2. Remove this object from its current parent's children list
        if (m_Parent != nullptr)
        {
            auto& siblings = m_Parent->m_Children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
        }

        // 3. Set the new parent
        m_Parent = newParent;

        // 4. Add this object to the new parent's children list
        if (m_Parent != nullptr)
        {
            // Ensure not adding duplicates, though erase/push_back handles typical scenarios
            m_Parent->m_Children.push_back(this);
        }

        // 5. Adjust local transform based on keepWorldPosition
        if (currentObjectTransform)
        {
            if (keepWorldPosition)
            {
                glm::mat4 newParentWorldMatrix(1.0f);
                if (m_Parent != nullptr)
                {
                    dae::TransformComponent* parentTransformComponent = m_Parent->GetTransform();
                    if (parentTransformComponent)
                    {
                        newParentWorldMatrix = parentTransformComponent->GetWorldTransform();
                    }
                }

                glm::mat4 newLocalMatrix = glm::inverse(newParentWorldMatrix) * oldWorldMatrix;

                // Decompose newLocalMatrix to set local position, rotation, and scale
                glm::vec3 newLocalPosition = glm::vec3(newLocalMatrix[3]);

                // Extract scale
                glm::vec3 newLocalScale{};
                newLocalScale.x = glm::length(glm::vec3(newLocalMatrix[0]));
                newLocalScale.y = glm::length(glm::vec3(newLocalMatrix[1]));
                newLocalScale.z = glm::length(glm::vec3(newLocalMatrix[2]));

                // Avoid division by zero if scale is zero (though typically scale shouldn't be zero)
                if (newLocalScale.x == 0.f) newLocalScale.x = 1e-6f;
                if (newLocalScale.y == 0.f) newLocalScale.y = 1e-6f;
                if (newLocalScale.z == 0.f) newLocalScale.z = 1e-6f;

                // Extract rotation
                // Remove scale from the matrix to get a pure rotation matrix
                const glm::mat3 rotationMatrix = glm::mat3(
                    glm::vec3(newLocalMatrix[0]) / newLocalScale.x,
                    glm::vec3(newLocalMatrix[1]) / newLocalScale.y,
                    glm::vec3(newLocalMatrix[2]) / newLocalScale.z
                );
                glm::quat newLocalRotation = glm::quat_cast(rotationMatrix);

                currentObjectTransform->SetLocalPosition(newLocalPosition);
                currentObjectTransform->SetLocalRotation(newLocalRotation);
                currentObjectTransform->SetLocalScale(newLocalScale);
            }
            else // !keepWorldPosition: Reset local transform relative to the new parent
            {
                currentObjectTransform->SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
                currentObjectTransform->SetLocalRotation(glm::quat()); // Identity quaternion
                currentObjectTransform->SetLocalScale(glm::vec3(1.0f, 1.0f, 1.0f));
            }
            // The SetLocal already sets dirty flag
        }
    }
}