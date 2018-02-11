#include "include/spring_system.h"
#include "include/shape_vertices.h"

#define RADIUS .2f
#define GRAVITY highp_dvec3(0, -9.81, 0)

SpringSystem::SpringSystem() :
    SpringSystem(10, 10, 50, 10) {}

SpringSystem::SpringSystem(int dimx, int dimy, double ks, double kd) {
    dimX_ = dimx;
    dimY_ = dimy;
    numNodes_ = dimX_ * dimY_;
    numTris_ = 2 * (dimX_ - 1) * dimY_;
    nodes_ = new Node[numNodes_];

    KS_ = ks;
    KD_ = kd;
    mass_ = .40;
    restLength_ = 0.5;

    initDX_ = 0.60;
    initDY_ = 1.00;
}

void SpringSystem::SpringSetup() {
    for (int r = 0; r < dimY_; r++) {
        for (int c = 0; c < dimX_; c++) {
            Node& n = GetNode(r, c);
            n.pos = highp_dvec3(c * initDX_, 5 - r*initDY_, 0);
            if (r != 0)
                n.vel = highp_dvec3(0, 0, 0);
        }
    }
}

void SpringSystem::GetPositions() {
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            Node& n = GetNode(r, c);
            posArray_[r*dimX_ + c] = n.pos;
        }
    }
}

/*
void SpringSystem::RecalculateNormals() {
    // reset normals
    for (int r = 0; r < dimY_; r++)
        for (int c = 0; c < dimX_; c++)
            normals_[r*dimX_ + c] = vec3(0, 0, 0);

    int i = 0;
    for (unsigned int r = 0; r < dimY_ - 1; ++r) {
        for (unsigned int c = 0; c < dimX_ - 1; ++c) {
            vec3 ul = posArray_[indices_[i + 0]];
            vec3 ll = posArray_[indices_[i + 1]];
            vec3 ur = posArray_[indices_[i + 2]];
            vec3 lr = posArray_[indices_[i + 5]];
            vec3 e12 = ll - ul;
            vec3 e13 = ur - ul;
            vec3 e34 = lr - ur;
            vec3 norm1 = cross(e12, e13);
            vec3 norm2 = cross(-e13, e34);
            normals_[(r + 0) * dimX_ + (c + 0)] += norm1;
            normals_[(r + 1) * dimX_ + (c + 0)] += norm1;
            normals_[(r + 0) * dimX_ + (c + 1)] += norm1;
            normals_[(r + 0) * dimX_ + (c + 1)] += norm2;
            normals_[(r + 1) * dimX_ + (c + 0)] += norm2;
            normals_[(r + 1) * dimX_ + (c + 1)] += norm2;
            i += 6;
        }
    }
    for (int r = 0; r < dimY_; r++)
        for (int c = 0; c < dimX_; c++)
            normals_[r*dimX_ + c] = normalize(normals_[r*dimX_ + c]);
}
*/

void SpringSystem::GLSetup() {
    shader_.LoadFromFile(GL_VERTEX_SHADER, "shaders/cloth_shader.vert");
    shader_.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/cloth_shader.frag");
    shader_.CreateAndLinkProgram();
    shader_.Enable();
    shader_.AddAttribute("inPos");
    shader_.AddAttribute("texCoords");
    shader_.AddUniform("VP");

    // allocate buffers
    posArray_ = new vec3[numNodes_];
    // normals_ = new vec3[numNodes_];
    texCoords_ = new vec2[numNodes_];
    indices_ = new unsigned int[3 * numTris_];

    // fill tex Coords buffer
    int i = 0;
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            float x = c / (float) dimX_;
            float y = 1.0f - r / (float) dimY_;
            texCoords_[i++] = vec2(x, y);
        }
    }
    // fill index buffer
    i = 0;
    for (unsigned int r = 0; r < dimY_ - 1; ++r) {
        for (unsigned int c = 0; c < dimX_ - 1; ++c) {
            unsigned int ul = (r + 0) * dimX_ + (c + 0);
            unsigned int ll = (r + 1) * dimX_ + (c + 0);
            unsigned int ur = (r + 0) * dimX_ + (c + 1);
            unsigned int lr = (r + 1) * dimX_ + (c + 1);
            indices_[i++] = ul;
            indices_[i++] = ll;
            indices_[i++] = ur;
            indices_[i++] = ur;
            indices_[i++] = ll;
            indices_[i++] = lr;
        }
    }

    glGenVertexArrays(1, &cloth_vao_);
    glBindVertexArray(cloth_vao_);
    glGenBuffers(CLOTH_TOTAL_VBOS, cloth_vbos_);

    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, cloth_vbos_[CLOTH_VERTS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * numNodes_, NULL, GL_STREAM_DRAW);
    glEnableVertexAttribArray(shader_["inPos"]);
    glVertexAttribPointer(shader_["inPos"], 3, GL_FLOAT, GL_FALSE, 0, 0);

    // tex coords
    glBindBuffer(GL_ARRAY_BUFFER, cloth_vbos_[CLOTH_TEX_COORDS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * numNodes_, &texCoords_[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(shader_["texCoords"]);
    glVertexAttribPointer(shader_["texCoords"], 2, GL_FLOAT, GL_FALSE, 0, 0);

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloth_vbos_[CLOTH_INDICES]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTris_ * 3,
                 &indices_[0], GL_STATIC_DRAW);

    cloth_texture_ = LoadTexture("textures/blue_cloth.jpg");
    shader_.AddUniform("tex");
}

void SpringSystem::Setup() {
    SpringSetup();
    GLSetup();
}

void SpringSystem::Update(double dt) {
    highp_dvec3 accels[dimY_][dimX_];
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            accels[r][c] = GRAVITY * mass_;
        }
    }
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
            acc += stringF * dir + dampF; // + GRAVITY * mass_;
            acc *= 1.0/mass_;

            accels[r][c] += acc;
            accels[r-1][c] -= acc;
        }
    }
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 1; c < dimX_; ++c) {
            Node& n1 = GetNode(r, c);
            Node& n2 = GetNode(r, c - 1);
            highp_dvec3 dpos = n1.pos - n2.pos;
            double stringLen = length(dpos);
            highp_dvec3 dir = normalize(dpos);
            double stringF = -KS_*(stringLen - restLength_);
            highp_dvec3 dampF = -KD_*(n1.vel - n2.vel);
            highp_dvec3 acc = highp_dvec3(0, 0, 0);
            acc += stringF * dir + dampF; // +  GRAVITY * mass_;
            acc *= 1.0/mass_;

            accels[r][c] += acc;
            accels[r][c-1] -= acc;
        }
    }

    accels[0][0] = vec3(0, 0, 0);
    accels[0][dimX_ - 1] = vec3(0, 0, 0);
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_; ++c) {
            Node& n = GetNode(r, c);
            n.vel += accels[r][c] * dt;
            n.pos += n.vel * dt;
        }
    }
}

void SpringSystem::Render(const mat4& VP) {
    shader_.Enable();
    glBindVertexArray(cloth_vao_);

    GetPositions();
    glBindBuffer(GL_ARRAY_BUFFER, cloth_vbos_[CLOTH_VERTS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * numNodes_, &posArray_[0], GL_STREAM_DRAW);

    glUniformMatrix4fv(shader_["VP"], 1, GL_FALSE, value_ptr(VP));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cloth_texture_);
    glUniform1i(shader_["tex"], 0);
    glDrawElements(GL_TRIANGLES, 3 * numTris_, GL_UNSIGNED_INT, 0);

    /*
    vec3 pArray[dimY_][dimX_];
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

    int numLines = 2 * (dimX_ * (dimY_ - 1) + dimY_ * (dimX_ - 1));
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
    for (int r = 0; r < dimY_; ++r) {
        for (int c = 0; c < dimX_ - 1; ++c) {
            vec3 p1 = pArray[r][c];
            vec3 p2 = pArray[r][c + 1];
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
    */
}
