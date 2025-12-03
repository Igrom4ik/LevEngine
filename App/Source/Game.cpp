#include "Game.hpp"
#include "TestObject.hpp"

// Enable stb_image single-file implementation for this translation unit

#include <filesystem>

#include "Core/scene/components/LightComponent.hpp"

bool Game::Init() {
	auto &fs = LEN::Engine::GetInstance().GetFileSystem();
	auto texture = LEN::Texture::Load("textures/brick.png");

	m_scene = new LEN::Scene();

	auto camera = m_scene->CreateObject("Camera");
	camera->AddComponent(new LEN::CameraComponent);
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
	camera->AddComponent(new LEN::PlayerControllerComponent);

	m_scene->SetMainCamera(camera);

	m_scene->CreateObject<TestObject>("TestObject");

	auto cubeMaterial = LEN::Material::Load("materials/brick.mat");
	auto suzanneMaterial = LEN::Material::Load("materials/suzanne.mat");

	auto mesh = LEN::Mesh::CreateQube();

	auto objectA = m_scene->CreateObject("ObjectA");
	objectA->AddComponent(new LEN::MeshComponent(cubeMaterial, mesh));
	objectA->SetPosition(glm::vec3(1.0f, 0.0f, -5.0f));

	auto objectB = m_scene->CreateObject("ObjectB");
	objectB->AddComponent(new LEN::MeshComponent(cubeMaterial, mesh));
	objectB->SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
	objectB->SetRotation(glm::vec3(0.0f, 2.0f, 0.0f));

	auto objectC = m_scene->CreateObject("ObjectC");
	objectC->AddComponent(new LEN::MeshComponent(cubeMaterial, mesh));
	objectC->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
	objectC->SetRotation(glm::vec3(1.0f, 2.0f, 1.0f));
	objectC->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

	auto suzanneMesh = LEN::Mesh::Load("models/Suzanne.gltf");

	auto suzanneObject = m_scene->CreateObject("Suzanne");
	suzanneObject->AddComponent(new LEN::MeshComponent(suzanneMaterial, suzanneMesh));
	suzanneObject->SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));

	auto light = m_scene->CreateObject("Light");
	auto lightComp = new LEN::LightComponent;
	lightComp->setColor(glm::vec3(1.0f));
	light->AddComponent(lightComp);
	light->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));

	LEN::Engine::GetInstance().SetScene(m_scene);
	return true;
}

void Game::Update(float deltaTime) {
	m_scene->Update(deltaTime);
}

void Game::Destroy() {
}
