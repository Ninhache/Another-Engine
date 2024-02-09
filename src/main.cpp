#include "main.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "headers/scene.hpp"
#include "headers/logger.hpp"

#include <iostream>

int main() {
    
    logger.log("Before render");
    Scene sc = Scene(800,600);
    sc.renderLoop();

    return 0;
}
