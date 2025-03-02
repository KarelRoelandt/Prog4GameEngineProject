#include "GameObject.h"
//#include "TransformComponent.h"

void dae::GameObject::Update(float deltaTime)
{
    for (auto& componentPair : m_Components)
    {
        componentPair.second->Update(deltaTime);
    }
}

void dae::GameObject::Render() const
{
    for (const auto& componentPair : m_Components)
    {
        componentPair.second->Render();
    }
}

void dae::GameObject::SetName(const std::string& name)
{
    m_Name = name;
}

const std::string& dae::GameObject::GetName() const
{
    return m_Name;
}

// Add the definition of GetParent
dae::GameObject* dae::GameObject::GetParent() const
{
    return m_Parent;
}

void dae::GameObject::SetParent(GameObject* parent)
{

	/// positie van aanpassen + checks toevoegen

    if (m_Parent)
    {
        // Remove this object from the current parent's children list
        auto& siblings = m_Parent->m_Children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    m_Parent = parent;

    if (m_Parent)
    {
        // Add this object to the new parent's children list
        m_Parent->m_Children.push_back(this);
    }
}