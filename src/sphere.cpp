#include "include/sphere.h"

Sphere::Sphere() : Sphere(glm::vec3(0,0,0), 1) {}

Sphere::Sphere(glm::vec3 pos, float r) {
    position = pos;
    radius = r;
    velocity = glm::vec3(0);
}

Sphere::~Sphere() {
}

void Sphere::Update(float dt) {
    position += dt * velocity * 2;
}

glm::mat4 Sphere::GetModelMatrix() {
    glm::mat4 model(1);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius, radius, radius));

    return model;
}
