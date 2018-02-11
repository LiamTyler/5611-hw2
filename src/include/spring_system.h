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

#define CLOTH_VERTS 0
#define CLOTH_NORMS 1
#define CLOTH_TEX_COORDS 2
#define CLOTH_INDICES 3
#define CLOTH_TOTAL_VBOS 4

class SpringSystem {
    public:
        SpringSystem();
        SpringSystem(int dimx, int dimy, double ks, double kd);
        void Setup();
        void SpringSetup();
        void GLSetup();
        void Update(double dt);
        void Render(const mat4& VP);
        Node& GetNode(int r, int c) { return nodes_[r*dimX_ + c]; }

        void GetPositions();
        // void RecalculateNormals();
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

        int numNodes_;
        int numTris_;
        bool textured_;
        vec3* posArray_;
        //vec3* normals_;
        vec2* texCoords_;
        unsigned int* indices_;

        // opengl shit
        GLSLShader shader_;
        GLint cloth_texture_;
        GLuint cloth_vao_;
        GLuint cloth_vbos_[CLOTH_TOTAL_VBOS];
        GLuint cloth_vert_vbo_;
        GLuint cloth_tex_vbo_;
        GLuint cloth_element_buffer_;
};

#endif  // SRC_INCLUDE_SPRING_SYSTEM_H_
