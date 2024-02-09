#include "headers/scene.hpp"
#include "headers/texture.hpp"
#include "headers/logger.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

float deltaTime = 0;
bool firstMouse = true;
float lastX, lastY;

bool camera_control = false;

uint16_t Scene::width = 800;
uint16_t Scene::height = 600;

Scene::Scene(uint16_t width, uint16_t height) {
	Scene::width = width;
	Scene::height = height;

	this->window = this->initWindow();
}

Scene::~Scene() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

std::string getPlatform() {
	#if defined(_WIN32) || defined(_WIN64)
		return "Windows";
	#elif defined(__linux__)
		return "Windows";
	#else
		return "Unknown operating system";
	#endif
}


std::string getGLString(GLenum name) {
    const GLubyte* str = glGetString(name);
    if (str) {
        return std::string(reinterpret_cast<const char*>(str));
    } else {
        return std::string();
    }
}

GLFWwindow* Scene::initWindow() {
	Scene::initGLFW();
	GLFWwindow* window = Scene::createWindow();
	Scene::initGLAD();

	std::string logMessage = "Another-Engine is running on " + getPlatform() + "\n"
							 + "Main scene has been initialized using " + getGLString(GL_VERSION) + "\n"
							 + "CPU: " + getGLString(GL_VENDOR) + "\n"
							 + "GPU: " + getGLString(GL_RENDERER) + "\n";
	logger.log(logMessage);

	return window;
}

void Scene::initGLFW() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* Scene::createWindow() {

	GLFWwindow* window = glfwCreateWindow(Scene::width, Scene::height, "Another-Engine", NULL, NULL);

	if (window == NULL) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Define the created window as the main context
	glfwMakeContextCurrent(window);

	/*We have to tell OpenGL the size of the rendering window so
	OpenGL knows how we want to display the data and coordinates with respect to the window*/
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	return window;
}

void Scene::initGLAD() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		exit(EXIT_FAILURE);
	}
}

void Scene::renderLoop() {
	this->setupScene();

	double current = 0;
	float lastFrame = 0.0f;
	int frame = 0;

	glfwSetCursorPosCallback(this->window, mouse_callback);
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(this->window, key_callback);

	Shader* defaultShader = this->shaders.find("default")->second;

	// Temp. data to test texture feature
	float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
	
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

	Texture texture1 = Texture::getTextureFromFile(std::string("textures/container.jpg"), aiTextureType_UNKNOWN, false);
	Texture texture2 = Texture::getTextureFromFile(std::string("textures/awesomeface.png"), aiTextureType_UNKNOWN, true);

	defaultShader->use();
    glUniform1i(glGetUniformLocation(defaultShader->getId(), "texture1"), 0);
    defaultShader->setInt("texture2", 1);

	while (!glfwWindowShouldClose(window)) {
		current = glfwGetTime();
		deltaTime = current - lastFrame;
		lastFrame = current;

		glClearColor(0.f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // raw use to draw textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1.getID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2.getID());

		glm::mat4 transform = glm::mat4(1.0f);
		unsigned int transformLoc = glGetUniformLocation(defaultShader->getId(), "transform");

        // fourth container
		transform = glm::mat4(1.0f); // reset it to identity matrix
        transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
        glfwPollEvents();
	}

}

void Scene::setupScene() {
	// this->addLight(new PointLight(glm::vec3(17.0f, 17.0f, -20.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 0.5f, 0.4f,1.0f,0.014, 0.0007));
	// this->addLight(new DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.5f, 0.5f, 0.5f), 0.5, 0.5));
	// this->addModel(new Model("models/backpack/backpack.obj", glm::vec3(0.0f, -2.0f, 0.0f)));

	this->addShader("default", new Shader{ "shaders/default.vs", "shaders/default.fs" });
}

void Scene::addShader(std::string name, Shader* shader) {
	this->shaders.insert({ name, shader });
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        if (!camera_control) {
                if (firstMouse) {
                        lastX = xpos;
                        lastY = ypos;
                        firstMouse = false;
                }

                float xoffset = xpos - lastX;
                float yoffset = lastY - ypos;
                lastX = xpos;
                lastY = ypos;

 //               camera.mouseUpdate(xoffset, yoffset);
        }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		camera_control = !camera_control;

		if (!camera_control) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

std::map<std::string, Shader*> Scene::getShaders() {
	return this->shaders;
}