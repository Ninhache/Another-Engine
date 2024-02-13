#include "headers/material.hpp"

Material* Material::withAmbient(glm::vec3 ambient) {
    this->ambient = ambient;
    return this;
}

Material* Material::withDiffuse(glm::vec3 diffuse) {
    this->diffuse = diffuse;
    return this;
}

Material* Material::withSpecular(glm::vec3 specular) {
    this->specular = specular;
    return this;
}

Material* Material::withShininess(float shininess) {
    this->shininess = shininess;
    return this;
}

glm::vec3 Material::getAmbient() {
    return ambient;
}

glm::vec3 Material::getDiffuse() {
    return diffuse;
}

glm::vec3 Material::getSpecular() {
    return specular;
}

float Material::getRealShininess() {
    return shininess;
}

float Material::getShininess() {
    return shininess * 128;
}