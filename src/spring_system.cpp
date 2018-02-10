#include "include/spring_system.h"
#include "include/shape_vertices.h"

#define RADIUS .2f
#define GRAVITY highp_dvec3(0, -9.81, 0)

SpringSystem::SpringSystem() :
    SpringSystem(10, 10, 50, 10) {}

SpringSystem::SpringSystem(int dimx, int dimy, double ks, double kd) {
    dimX_ = dimx;
    dimY_ = dimy;
    nodes_ = new Node[dimX_ * dimY_];

    KS_ = ks;
    KD_ = kd;
    mass_ = 4.0;
    restLength_ = 0.05;

    initDX_ = 1.0;
    initDY_ = 0.2;
}

void SpringSystem::SpringSetup() {
    for (int r = 0; r < dimY_; r++) {
        for (int c = 0; c < dimX_; c++) {
            Node& n = GetNode(r, c);
            n.pos = highp_dvec3(c * initDX_, 5 - r*initDY_, 0);
            n.vel = highp_dvec3(0, 0, 0);
        }
    }
}

void SpringSystem::GLSetup(GLSLShader& shader) {
    glGenVertexArrays(1, &cube_vao_);
    glBindVertexArray(cube_vao_);
    glGenBuffers(1, &cube_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shader["normals"]);
    glVertexAttribPointer(shader["normals"], 3, GL_FLOAT, GL_FALSE,
            0, (void*) CUBE_VERTS_SIZE);
    glEnableVertexAttribArray(shader["texCoords"]);
    glVertexAttribPointer(shader["texCoords"], 2, GL_FLOAT, GL_FALSE,
            0, (void*) (CUBE_VERTS_SIZE + CUBE_NORMS_SIZE));

    glGenVertexArrays(1, &springs_vao_);
    glBindVertexArray(springs_vao_);
    glGenBuffers(1, &springs_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, springs_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 2 * dimX_ * (dimY_ - 1), NULL, GL_STREAM_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SpringSystem::Setup(GLSLShader& shader) {
    SpringSetup();
    GLSetup(shader);
}

void SpringSystem::Update(double dt) {
    highp_dvec3 accels[dimY_][dimX_] = { highp_dvec3(0, 0, 0) };
    for (int r = 1; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            Node& n1 = GetNode(r, c);
            Node& n2 = GetNode(r - 1, c);
            highp_dvec3 dpos = n1.pos - n2.pos;
            double stringLen = length(dpos);
            highp_dvec3 dir = normalize(dpos);
            double stringF = -KS_*(stringLen - restLength_);
            highp_dvec3 dampF = -KD_*(n1.vel - n2.vel);
            highp_dvec3 acc = highp_dvec3(0, 0, 0);
            acc += stringF * dir + dampF +  GRAVITY * mass_;
            acc *= 1.0/mass_;

            accels[r][c] += acc;
            accels[r-1][c] -= acc;
        }
    }

    for (int r = 1; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            Node& n = GetNode(r, c);
            n.vel += accels[r][c] * dt;
            n.pos += n.vel * dt;
        }
    }
}

void SpringSystem::Render(GLSLShader& shader) {
    vec3 pArray[dimY_][dimX_];
    mat4 model;
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            Node& n = GetNode(r, c);
            pArray[r][c] = n.pos;
        }
    }
    glUniform1i(shader["textured"], false);
    glBindVertexArray(cube_vao_);
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            model = mat4(1);
            model = translate(model, pArray[r][c]);
            model = scale(model, RADIUS * vec3(1, 1, 1));
            glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    int numLines = 2 * dimX_ * (dimY_ - 1);
    vec3 lineArray[numLines];
    int line = 0;
    for (int r = 0; r < dimY_ - 1; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            vec3 p1 = pArray[r][c];
            vec3 p2 = pArray[r+1][c];
            lineArray[line++] = p1;
            lineArray[line++] = p2;
        }
    }

    glUniform1i(shader["textured"], false);
    glBindVertexArray(springs_vao_);
    model = mat4(1);
    glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
    glBindBuffer(GL_ARRAY_BUFFER, springs_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * numLines, lineArray, GL_STREAM_DRAW);
    glDrawArrays(GL_LINES, 0, numLines);
}
