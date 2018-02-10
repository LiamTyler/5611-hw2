#ifndef SRC_INCLUDE_SPRING_SYSTEM_H_
#define SRC_INCLUDE_SPRING_SYSTEM_H_

#include "include/utils.h"
#include "include/glsl_shader.h"

typedef struct Node {
    Node() {
        pos = highp_dvec3(0, 0, 0);
        vel = highp_dvec3(0, 0, 0);
    }

    Node(vec3 p, vec3 v) {
        pos = p;
        vel = v;
    }

    highp_dvec3 pos;
    highp_dvec3 vel;

} Node;

class SpringSystem {
    public:
        SpringSystem();
        SpringSystem(int dimx, int dimy, double ks, double kd);
        void Setup(GLSLShader& shader);
        void SpringSetup();
        void GLSetup(GLSLShader& shader);
        void Update(double dt);
        void Render(GLSLShader& shader);
        Node& GetNode(int r, int c) { return nodes_[r*dimX_ + c]; }

        int DimX() { return dimX_; }
        int DimY() { return dimY_; }

    private:
        int dimX_;
        int dimY_;
        double KS_;
        double KD_;
        Node* nodes_;

        double initDX_;
        double initDY_;
        double mass_;
        double restLength_;

        // opengl shit
        GLuint cube_vao_;
        GLuint cube_vbo_;
        GLuint springs_vao_;
        GLuint springs_vbo_;
};

#endif  // SRC_INCLUDE_SPRING_SYSTEM_H_
