#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "map"

#include "camera.hpp"
#include "shader.hpp"

class Scene {

public:

    Scene(uint16_t width, uint16_t height);

    /**
     * @brief Destructor cleans the libraries
    */
    ~Scene();

    /**
     * @brief Setup a callback for when the user press a key
     *
     */
    void initWindowKeyCallback();

    /**
     * @brief Launch the render loop, doesnt initialize anything, just launching the loop
     *
     */
    void renderLoop();

    /**
     * @brief Add things to the scene to have something to render
    */
    void setupScene();

    void addShader(std::string name, Shader* shader);
    std::map<std::string, Shader*> getShaders();

    static uint16_t width;
    static uint16_t height;

private:
    GLFWwindow* window;    
    std::map<std::string, Shader*> shaders;

    /**
     * @brief Init all the libraries and generate a windows
     *
     * @return GLFWwindow*
     */
    GLFWwindow* initWindow();

    /**
     * @brief init the GLFW Library :
     * GLFW provides a simple API for creating windows, contexts and surfaces, receiving input and events
     */
    void initGLFW();

    /**
     * @brief create a window from glfw
     *
     * @return GLFWwindow* program can exit if window finish as null
     */
    GLFWwindow* createWindow();

    /**
     * @brief Init the GLAD Library :
     * GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function.
     */
    void initGLAD();

    /**
     * @brief Setup a callback for when the user resize the window
     *
     * @param window
     * @param width
     * @param height
     */
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        Scene::width = width;
        Scene::height = height;
    }

};

extern Camera camera;