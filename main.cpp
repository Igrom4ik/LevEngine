#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>


int main()
{
    /************************************************************************
     *                          INITIALIZE GLFW                              *
     *  - Initialize the GLFW library and exit on failure                     *
     ************************************************************************/
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    /************************************************************************
     *            WINDOW HINTS / REQUEST OPENGL CONTEXT                      *
     *  - Request an OpenGL 3.3 Core Profile context                          *
     ************************************************************************/
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /************************************************************************
     *                             CREATE WINDOW                              *
     *  - Create the application window and exit if creation fails            *
     ************************************************************************/
    GLFWwindow* window = glfwCreateWindow(1200, 800, "LevEngine", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /************************************************************************
     *                    SETUP CONTEXT AND WINDOW POSITION                   *
     *  - Set the window position and make its OpenGL context current         *
     ************************************************************************/
    glfwSetWindowPos(window, 2000, 150);
    glfwMakeContextCurrent(window);

    /************************************************************************
     *                          INITIALIZE GLEW                               *
     *  - Initialize GLEW after making a valid OpenGL context current         *
     ************************************************************************/
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }


    /************************************************************************
     *                 SHADER SOURCES AND COMPILATION                         *
     *  - Define vertex and fragment shader source strings                    *
     *  - Compile shaders, check for compile errors and link the program      *
     ************************************************************************/

    std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 position;
        layout (location = 1) in vec3 color;

        out vec3 vColor;

        void main()
        {
            vColor = color;
            gl_Position = vec4(position.x, position.y, position.z, 1.0);
        }
    )";

	GLuint vertexShader  = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderCStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderCStr, nullptr);
	glCompileShader(vertexShader);

    GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
		char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }



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

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSCStr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSCStr, nullptr);
	glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];  
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" <<
			infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    /************************************************************************
     *                              VERTEX DATA                              *
     *  - Define triangle vertex coordinates (can be uploaded to GPU later)   *
     ************************************************************************/
    std::vector<float> vertices =
    {


		 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // Top vertex (Red)
		 -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom left vertex (Green)
		 -0.5f,  -0.5f, 0.0f , 0.0f, 0.0f, 1.0f,  // Bottom right vertex (Blue)
		 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,  // Top vertex (Red)
    };

	// Indices for two triangles (if using EBO) — optimized way for calling vertices
    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        0, 2, 3
	};


    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW); // Upload vertex data
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); // Upload index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind EBO

    GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind VBO (it is stored in the VAO now)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // Bind EBO (it is stored in the VAO now)

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLuint uColorLoc =  glGetUniformLocation(shaderProgram, "uColor");


    /************************************************************************
     *                            RENDER LOOP / MAIN LOOP                      *
     *  - Rendering loop: clear screen, swap buffers and poll events          *
     ************************************************************************/
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glUniform4f(uColorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /************************************************************************
     *                                 CLEANUP                                *
     *  - Terminate GLFW and release resources                                *
     ************************************************************************/
    glfwTerminate();
    return 0;
}
