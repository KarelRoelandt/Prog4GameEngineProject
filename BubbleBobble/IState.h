#pragma once

namespace dae
{
    class GameObject;

    class IState
    {
    public:
        virtual ~IState() = default;
        virtual void Enter(GameObject* object) = 0;
        virtual void Update(GameObject* object, float deltaTime) = 0;
        virtual void Exit(GameObject* object) = 0;
    };
}