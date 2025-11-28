#include "Game.hpp"
#include "TestObject.hpp"

// Enable stb_image single-file implementation for this translation unit

#include <filesystem>

bool Game::Init() {
	auto &fs = LEN::Engine::GetInstance().GetFileSystem();
	auto texture = LEN::Texture::Load("brick.png");

	m_scene = new LEN::Scene();

	auto camera = m_scene->CreateObject("Camera");
	camera->AddComponent(new LEN::CameraComponent);
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));
	camera->AddComponent(new LEN::PlayerControllerComponent);

	m_scene->SetMainCamera(camera);

	m_scene->CreateObject<TestObject>("TestObject");

	std::string vertexShaderSource = fs.LoadAssetTextFile("shaders/vertex.glsl");
	std::string fragmentShaderSource = fs.LoadAssetTextFile("shaders/fragment.glsl");


	auto &graphicAPI = LEN::Engine::GetInstance().GetGraphicsAPI();
	auto shaderProgram = graphicAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);


	auto material = std::make_shared<LEN::Material>();
	material->SetShaderProgram(shaderProgram);
	material->SetParam("brickTexture", texture);

	std::vector<float> vertices = {
		// Front face (z = 0.5)
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Top face (y = 0.5)
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Right face (x = 0.5)
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Left face (x = -0.5)
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Bottom face (y = -0.5)
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// Back face (z = -0.5)
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f
	};

	std::vector<uint32_t> indices = {
		// Front face
		0, 1, 2,
		0, 2, 3,
		// Top face
		4, 5, 6,
		4, 6, 7,
		// Right face
		8, 9, 10,
		8, 10, 11,
		// Left face
		12, 13, 14,
		12, 14, 15,
		// Back face
		16, 17, 18,
		16, 18, 19,
		// Bottom face
		20, 21, 22,
		20, 22, 23
	};

	LEN::VertexLayout vertexLayout;
	// Position attribute
	vertexLayout.elements.push_back({0, 3, GL_FLOAT, 0});
	// Color attribute
	vertexLayout.elements.push_back({1, 3, GL_FLOAT, sizeof(float) * 3});
	// UV
	vertexLayout.elements.push_back({2, 2, GL_FLOAT, sizeof(float) * 6});

	// Stride: total size of one vertex (position + color)
	vertexLayout.stride = sizeof(float) * 8;

	auto mesh = std::make_shared<LEN::Mesh>(vertexLayout, vertices, indices);

	auto objectA = m_scene->CreateObject("ObjectA");
	objectA->AddComponent(new LEN::MeshComponent(material, mesh));
	objectA->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));

	auto objectB = m_scene->CreateObject("ObjectB");
	objectB->AddComponent(new LEN::MeshComponent(material, mesh));
	objectB->SetPosition(glm::vec3(0.0f, 2.0f, 2.0f));
	objectB->SetRotation(glm::vec3(0.0f, 2.0f, 0.0f));

	auto objectC = m_scene->CreateObject("ObjectC");
	objectC->AddComponent(new LEN::MeshComponent(material, mesh));
	objectC->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
	objectC->SetRotation(glm::vec3(1.0f, 2.0f, 1.0f));
	objectC->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));

	LEN::Engine::GetInstance().SetScene(m_scene);
	return true;
}

void Game::Update(float deltaTime) {
	m_scene->Update(deltaTime);
}

void Game::Destroy() {
}
