#pragma once

#define GLFW_VERSION_MAJOR 3
#define GLFW_VERSION_MINOR 4

/* Minimal placeholder for glfw3.h */
typedef void GLFWwindow;

#ifdef __cplusplus
extern "C" {
#endif

static inline int glfwInit(void) { return 1; }
static inline void glfwTerminate(void) {}

#ifdef __cplusplus
}
#endif

