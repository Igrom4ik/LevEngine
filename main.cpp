#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <iostream>


int main()
{
	// Initialize the GLFW library
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }


     /* ============================================================
     *  CREATE AND INITIALIZE WINDOW
     *
     * ============================================================ */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window =  glfwCreateWindow (1200, 800, "LevEngine", nullptr, nullptr);

    if (window == nullptr) {

		std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
		return -1;
    }

	glfwSetWindowPos(window, 2000, 150);
    glfwMakeContextCurrent(window);
    
    if(glewInit() != GLEW_OK)

	// Main loop
	while (!glfwWindowShouldClose(window)) {
        // Poll for and process events
        glfwPollEvents();
    }
	glfwTerminate();

    return 0;
}
