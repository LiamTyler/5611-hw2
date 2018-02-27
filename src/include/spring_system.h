#ifndef SRC_INCLUDE_SPRING_SYSTEM_H_
#define SRC_INCLUDE_SPRING_SYSTEM_H_

#include "include/utils.h"
#include "include/glsl_shader.h"
#include "include/sphere.h"

typedef struct Node {
    Node() {
        pos = vec3(0, 0, 0);
        vel = vec3(0, 0, 0);
    }

    Node(vec3 p, vec3 v) {
        pos = p;
        vel = v;
    }

    vec3 pos;
    vec3 vel;

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
        void SpringSetup(bool vertical);
        void GLSetup();
        void Update(double dt);
        void Render(const mat4& V, const mat4& P);
        void HandleCollisions(Sphere& sphere);
        Node& GetNode(int r, int c) { return nodes_[r*dimX_ + c]; }

        void ChangeVizualization() { textured_ = !textured_; }
        void Pause() { paused_ = !paused_; }
        void UpdateGPUPositions();
        void RecalculateNormals();
        int DimX() { return dimX_; }
        int DimY() { return dimY_; }
        void Drag(bool d) { drag_ = d; }
        bool Drag() { return drag_; }

        double GetKS() { return KS_; }
        double GetKD() { return KD_; }
        void SetKS(double ks) { KS_ = ks; }
        void SetKD(double kd) { KD_ = kd; }

        bool stuck;
    private:
        vec3 wind_;
        bool drag_;
        int dimX_;
        int dimY_;
        double KS_;
        double KD_;
        Node* nodes_;

        double initDX_;
        double initDY_;
        double mass_;
        double restLength_;

        bool textured_;
        bool paused_;

        int numNodes_;
        int numTris_;
        vec3* posArray_;
        vec3* normals_;
        vec2* texCoords_;
        unsigned int* indices_;
        vector<unsigned int> spring_indices_;

        // opengl shit
        GLSLShader cloth_shader_;
        GLSLShader spring_shader_;
        GLint cloth_texture_;
        GLuint cloth_vao_;
        GLuint cloth_vbos_[CLOTH_TOTAL_VBOS];
        GLuint cube_vao_;
        GLuint cube_vbo_;
        GLuint spring_vao_;
        GLuint spring_vbo_;
};

#endif  // SRC_INCLUDE_SPRING_SYSTEM_H_
