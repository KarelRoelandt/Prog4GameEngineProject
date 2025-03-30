#pragma once

#include "BaseComponent.h"

namespace dae {
    class ExampleComponent : public BaseComponent {
    public:
        void Update(float deltaTime) override;
        void Render() const override;
    };

    void ExampleComponent::Update(float deltaTime) {
        // Example usage of 'var'
        float var = deltaTime;
        // Use 'var' in some meaningful way
        if (var > 0) {
            // Perform some update logic
        }
        // Update logic here
    }


    void ExampleComponent::Render() const {
        // Render logic here
    }
}
