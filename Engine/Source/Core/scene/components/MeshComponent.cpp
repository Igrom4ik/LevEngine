//
// Created by igrom on 22.11.2025.
//

#include "MeshComponent.hpp"
#include "Core/render/Material.hpp"
#include "Core/render/Mesh.hpp"
#include "Core/render/RenderQueue.hpp"
#include "Core/scene/GameObject.hpp"
#include "Core/Engine.hpp"

namespace LEN {

    MeshComponent::MeshComponent(const std::shared_ptr<Material> &material, const std::shared_ptr<Mesh> &mesh)
        : m_material(material), m_mesh(mesh) {

        }

    void MeshComponent::Update(float deltaTime) {
        if (!m_material || !m_mesh) return;

        RenderCommand cmd;
        cmd.material = m_material.get();
        cmd.mesh = m_mesh.get();
        cmd.modelMatrix = GetOwner()->GetWorldTransform(); // Get the world transform from the owner GameObject

        auto& renderQueue = Engine::GetInstance().GetRenderQueue();
        renderQueue.Submit(cmd);
    }

}
