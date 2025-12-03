#pragma once

#include "Source/Core/scene/Component.hpp"
#include <glm/vec3.hpp>

namespace LEN {
    class LightComponent : public Component {
        COMPONENT(LightComponent)

    public:
        void Update(float deltaTime) override;

        void setColor(glm::vec3 color);

        const glm::vec3 &GetColor() const;

    private:
        glm::vec3 m_color = glm::vec3(1.0f);
        float m_intensity;
    };
}
