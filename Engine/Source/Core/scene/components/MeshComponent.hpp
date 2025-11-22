//
// Created by igrom on 22.11.2025.
//

#pragma once

#include <memory>

#include "Core/scene/Component.hpp"

namespace LEN {
    class Material;
    class Mesh;
    class MeshComponent : public Component {

    public:
        MeshComponent(const std::shared_ptr<Material>& material, const std::shared_ptr<Mesh>& mesh);
        void Update(float deltaTime) override;

    private:
        std::shared_ptr<Material> m_material;
        std::shared_ptr<Mesh> m_mesh;
    };
}

