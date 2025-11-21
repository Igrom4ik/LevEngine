#pragma once

#include <Core/eng.hpp>
#include <memory>

class TestObject : public LEN::GameObject
{
public:
	TestObject();
	void Update(float deltaTime) override;

private:
	LEN::Material m_material;
	std::shared_ptr<LEN::Mesh> m_mesh; // Using unique_ptr for automatic memory management
	float m_offsetX = 0.0f;
	float m_offsetY = 0.0f;
};