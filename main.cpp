#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>


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
     *                   WINDOW HINTS / OPENGL CONTEXT                       *
     *  - Request a 3.3 Core profile context                                  *
     ************************************************************************/
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /************************************************************************
     *                            CREATE WINDOW                               *
     *  - Create the GLFW window and exit if creation fails                   *
     ************************************************************************/
    GLFWwindow* window = glfwCreateWindow(1200, 800, "LevEngine", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /************************************************************************
     *                      SETUP CONTEXT & WINDOW POSITION                   *
     *  - Position the window and make its GL context current                 *
     ************************************************************************/
    glfwSetWindowPos(window, 2000, 150);
    glfwMakeContextCurrent(window);

    /************************************************************************
     *                             INITIALIZE GLEW                            *
     *  - Initialize GLEW after an OpenGL context has been made current       *
     ************************************************************************/
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    /************************************************************************
     *                              VERTEX DATA                               *
     *  - Define vertex positions (currently unused placeholder)              *
     ************************************************************************/
    std::vector<float> vertices =
    {
         0.0f, 0.5f, 0.0f,
         -0.5f, -0.5f, 0.0f,
         0.5f,  -0.5f, 0.0f
    };

    /************************************************************************
     *                                MAIN LOOP                               *
     *  - Clear the screen, swap buffers and poll events                      *
     ************************************************************************/
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /************************************************************************
     *                                 CLEANUP                                *
     *  - Terminate GLFW                                                      *
     ************************************************************************/
    glfwTerminate();
    return 0;
}
