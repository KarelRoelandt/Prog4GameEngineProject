// ScoreComponent.h
#pragma once
#include "BaseComponent.h"
#include "Observer.h"
#include "GameObject.h"

namespace dae
{
    class ScoreComponent final : public BaseComponent, public Subject
    {
    public:
        ScoreComponent(GameObject* owner, int initialScore = 0);
        ~ScoreComponent() override = default;

        void AddScore(int amount);
        void ResetScore();
        int GetScore() const { return m_score; }

        // Implement the abstract methods from BaseComponent
        void Update(float deltaTime) override;
        void Render() const override;

    private:
        int m_score;
    };
}