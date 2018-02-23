#ifndef SRC_INCLUDE_SPHERE_H_
#define SRC_INCLUDE_SPHERE_H_

#include "include/utils.h"

class Sphere {
    public:
        Sphere();
        Sphere(glm::vec3 pos, float r);
        ~Sphere();

        void Update(float dt);
        glm::mat4 GetModelMatrix();

        glm::vec3 position;
        glm::vec3 velocity;
        float radius;
};

#endif  // SRC_INCLUDE_SPHERE_H_
