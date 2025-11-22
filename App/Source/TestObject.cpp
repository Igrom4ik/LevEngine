#include "TestObject.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

TestObject::TestObject()
{

	std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        out vec3 vColor;

        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;


        void main()
        {
            vColor = color;
            gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
        }
    )";

	std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vColor;

        void main()
        {
            // Use vertex color directly to avoid relying on material uniform support for vec4
            FragColor = vec4(vColor, 1.0);
        }
    )";



	auto& graphicAPI = LEN::Engine::GetInstance().GetGraphicsAPI(); // Get GraphicsAPI instance
	auto shaderProgram = graphicAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

	auto material = std::make_shared<LEN::Material>();
	material->SetShaderProgram(shaderProgram); // Set the shader program to the material


	std::vector<float> vertices =
	{

		 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 -0.5f,  -0.5f, 0.0f , 0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	LEN::VertexLayout vertexLayout;

	// Position
	vertexLayout.elements.push_back({ 0, 3, GL_FLOAT, 0 });

	// Color
	vertexLayout.elements.push_back({ 1, 3, GL_FLOAT, sizeof(float) * 3 });
	vertexLayout.stride = sizeof(float) * 6; // 3 for position + 3 for color

	// Create Mesh
	auto mesh = std::make_shared<LEN::Mesh>(vertexLayout, vertices, indices);

	AddComponent(new LEN::MeshComponent(material, mesh));
}

void TestObject::Update(float deltaTime)
{
	LEN::GameObject::Update(deltaTime);

	auto position = GetPosition()
;	auto& input = LEN::Engine::GetInstance().GetInputManager();
	// Horizontal movement
	if (input.IsKeyPressed(LEN::Key::A) || input.IsKeyPressed(LEN::Key::Left))
	{
		position.x -= 0.01f;
	}
	else if (input.IsKeyPressed(LEN::Key::D) || input.IsKeyPressed(LEN::Key::Right))
	{
		position.x += 0.01f;
	}

	// Vertical movement
	if (input.IsKeyPressed(LEN::Key::S) || input.IsKeyPressed(LEN::Key::Down))
	{
		position.y -= 0.01f;
	}

	else if (input.IsKeyPressed(LEN::Key::W) || input.IsKeyPressed(LEN::Key::Up))
	{
		position.y += 0.01f;
	}
	SetPosition(position);

}

