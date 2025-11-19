#include "Game.hpp"
#include <iostream>

bool Game::Init()
{

    // ---------------------------------------------------------------------
    // Shaders — source strings
    // - Define GLSL source for the vertex and fragment shaders below.
    // - Compilation and linking are performed by the graphics backend when
    //   creating a shader program (see CreateShaderProgram usage further).
    // ---------------------------------------------------------------------

    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        uniform vec2 uOffset;

        out vec3 vColor;

        void main()
        {
            vColor = color;
            gl_Position = vec4(position.x + uOffset.x, position.y + uOffset.y, position.z, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 vColor;
        uniform vec4 uColor;        

        void main()
        {
            FragColor = vec4(vColor, 1.0f) * uColor;
        }
    )";

    // Acquire the GraphicsAPI instance and create a shader program.
    // The graphics backend will compile and link the provided GLSL sources
    // and return a shared pointer to the created ShaderProgram.
    // Keep the returned handle for use during rendering and uniform updates.

    auto& graphicAPI = LEN::Engine::GetInstance().GetGraphicsAPI(); // Get GraphicsAPI instance
    auto shaderProgram = graphicAPI.CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	m_material.SetShaderProgram(shaderProgram); // Set the shader program to the material


	// Create geometry data for a simple square (two triangles)
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

	m_mesh = std::make_unique<LEN::Mesh>(vertexLayout, vertices, indices);

    return true;
}

void Game::Update(float deltatime)
{
	auto& input = LEN::Engine::GetInstance().GetInputManager();

	if (input.IsKeyPressed(LEN::Key::A))
	{
		std::cout << "[A] button is pressed" << std::endl;
	}
}

void Game::Destroy()
{
	
}
