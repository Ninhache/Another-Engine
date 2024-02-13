#pragma once

#include <glm/glm.hpp>

// http://devernay.free.fr/cours/opengl/materials.html

class Material
{
public:

    static Material* create() {
        return new Material{};
    }

    Material* withAmbient(glm::vec3 ambient);
    Material* withDiffuse(glm::vec3 diffuse);
    Material* withSpecular(glm::vec3 specular);
    Material* withShininess(float shininess);

    glm::vec3 getAmbient();
    glm::vec3 getDiffuse();
    glm::vec3 getSpecular();

    // this function exist to have the "real" value of the shininess, between 0 and 1
    float getRealShininess();
    float getShininess();



private:
    // default value of "black plastic"
    glm::vec3 ambient   = glm::vec3(0.0f, 0.0f, 0.0f),
              diffuse   = glm::vec3(0.01f, 0.01f, 0.01f),
              specular  = glm::vec3(0.5f, 0.5f, 0.5f);
    float     shininess = 0.25;

    Material(){ }

	
};

