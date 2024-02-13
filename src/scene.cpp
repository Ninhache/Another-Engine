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

	glEnable(GL_DEPTH_TEST); 

	// Temp. data to test
	float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    Texture diffuseMap = Texture::getTextureFromFile(std::string("textures/container2.png"), aiTextureType_UNKNOWN, false);
    Texture specularMap = Texture::getTextureFromFile(std::string("textures/container2_specular.png"), aiTextureType_UNKNOWN, false);

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    
    Shader* lightShader = this->shaders.find("light")->second;
    Shader* cubeShader = this->shaders.find("cube")->second;
    Material* goldMaterial = this->materials.find("emerald")->second;

     // shader configuration
    // --------------------
    lightShader->use();
    lightShader->setInt("material.diffuse", 0);
    lightShader->setInt("material.specular", 1);

	while (!glfwWindowShouldClose(window)) {
		current = glfwGetTime();
		deltaTime = current - lastFrame;
		lastFrame = current;

        camera.processInput(window, deltaTime);
	    camera.update();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        lightShader->use();
        lightShader->setVec3("light.position", lightPos);
        lightShader->setVec3("viewPos", camera.getPos());

        // light properties
        lightShader->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        lightShader->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        lightShader->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // material properties
        lightShader->setFloat("material.shininess", 8.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.getLookAtMatrix();
        lightShader->setMatrix4("projection", projection);
        lightShader->setMatrix4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightShader->setMatrix4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap.getID());
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap.getID());

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        cubeShader->use();
        cubeShader->setMatrix4("projection", projection);
        cubeShader->setMatrix4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        cubeShader->setMatrix4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


		glfwSwapBuffers(window);
        glfwPollEvents();
	}

}

void Scene::setupScene() {
	// this->addLight(new PointLight(glm::vec3(17.0f, 17.0f, -20.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 0.5f, 0.4f,1.0f,0.014, 0.0007));
	// this->addLight(new DirectionalLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.5f, 0.5f, 0.5f), 0.5, 0.5));
	// this->addModel(new Model("models/backpack/backpack.obj", glm::vec3(0.0f, -2.0f, 0.0f)));

	this->addShader("light", new Shader{ "shaders/light.vs", "shaders/light.fs" });
    this->addShader("cube", new Shader{ "shaders/cube.vs", "shaders/cube.fs" });

    this->addMaterial("gold", Material::create()->withAmbient(glm::vec3(0.24725, 0.1995, 0.0745))
                                               ->withDiffuse(glm::vec3(0.75164, 0.60648, 0.22648))
                                               ->withSpecular(glm::vec3(0.628281, 0.555802, 0.366065))
                                               ->withShininess(0.4));


    this->addMaterial("emerald", Material::create()->withAmbient(glm::vec3(0.0215,0.1745,0.0215))
                                               ->withDiffuse(glm::vec3(0.07568,0.61424,0.07568))
                                               ->withSpecular(glm::vec3(0.633,0.727811,0.633))
                                               ->withShininess(0.6));
}

void Scene::addShader(std::string name, Shader* shader) {
	this->shaders.insert({ name, shader });
}

void Scene::addMaterial(std::string name, Material* material) {
	this->materials.insert({ name, material });
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

std::map<std::string, Material*> Scene::getMaterials() {
	return this->materials;
}