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

Camera camera;
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
		return "Linux";
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

	glEnable(GL_DEPTH_TEST); 

	// Temp. data to test texture feature
	float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

	unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	Texture texture1 = Texture::getTextureFromFile(std::string("textures/container.jpg"), aiTextureType_UNKNOWN, false);
	Texture texture2 = Texture::getTextureFromFile(std::string("textures/awesomeface.png"), aiTextureType_UNKNOWN, false);

	defaultShader->use();
    glUniform1i(glGetUniformLocation(defaultShader->getId(), "texture1"), 0);
    defaultShader->setInt("texture2", 1);

	while (!glfwWindowShouldClose(window)) {
		current = glfwGetTime();
		deltaTime = current - lastFrame;
		lastFrame = current;

		glClearColor(0.5f,0.f,0.f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1.getID());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2.getID());

        camera.processInput(window, deltaTime);
	    camera.update();

        glm::mat4 projection;					   //FOV	         //Aspect ratio                              //near //far plane frustum
	    projection = glm::perspective(glm::radians(camera.getFov()), (float)Scene::width / (float)Scene::height, 0.1f, 100.0f);

        //Model
        defaultShader->setVec3("viewPos", camera.getPos());
        defaultShader->setMatrix4("view", camera.getLookAtMatrix());
        defaultShader->setMatrix4("projection", projection);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i * glfwGetTime();
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            defaultShader->setMatrix4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

		defaultShader->use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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

               camera.mouseUpdate(xoffset, yoffset);
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