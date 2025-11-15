#include "pch.hpp"

#include <iostream>

// Попытка подключить заглушки GLFW/GLEW
#include <GLFW/glfw3.h>
#include <GL/glew.h>

int main() {
    std::cout << "LevEngine: minimal startup" << std::endl;

    // Проверка наличия символов из заглушек
#ifdef GLFW_VERSION_MAJOR
    std::cout << "GLFW header present" << std::endl;
#endif
#ifdef GLEW_VERSION
    std::cout << "GLEW header present" << std::endl;
#endif

    return 0;
}
