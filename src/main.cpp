#include "include/utils.h"
#include "include/camera.h"
#include "include/glsl_shader.h"
#include "include/image.h"
#include "include/fps_counter.h"
#include "include/mesh.h"
#include "include/shape_vertices.h"

using namespace std;

#define RADIUS .2f
#define dimX 4
#define dimY 10
#define initDX 1
#define initDY 0.2
#define LINE_WIDTH 2
#define REST_LENGTH 0.05
#define KS 50.0
#define KD 10.0
#define MASS 4.0
#define GRAVITY highp_dvec3(0, -9.81, 0)
#define DT 0.001

void print_lengths(void* data) {
    vec3* pos = (vec3*) data;
    // cout << "Lengths: " << endl;
}

int main(int arc, char** argv) {
    highp_dvec3 pos[dimY][dimX];
    highp_dvec3 vel[dimY][dimX];
    for (int r = 0; r < dimY; r++) {
        for (int c = 0; c < dimX; c++) {
            pos[r][c] = highp_dvec3(c*initDX, 5 - r*initDY, 0);
            vel[r][c] = highp_dvec3(0,0,0);
        }
    }

    // initialize SDL and GLEW and set up window
    SDL_Window* window = InitAndWindow("Starter Project", 100, 100, 800, 600);
    cout << "vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "version: " << glGetString(GL_VERSION) << endl;

    // set up the particle system
    Camera camera = Camera(vec3(4, 0, 15), vec3(0, 0, -1), vec3(0, 1, 0), 4, 0.01);
    GLSLShader shader;
    shader.LoadFromFile(GL_VERTEX_SHADER,   "shaders/plain_shader.vert");
    shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/plain_shader.frag");
    shader.CreateAndLinkProgram();
    shader.Enable();
    shader.AddAttribute("verts");
    shader.AddAttribute("normals");
    shader.AddAttribute("texCoords");

    shader.AddUniform("model");
    shader.AddUniform("VP");
    shader.AddUniform("textured");

    GLuint cube_vao;
    GLuint cube_vbo;
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_data), cube_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shader["normals"]);
    glVertexAttribPointer(shader["normals"], 3, GL_FLOAT, GL_FALSE,
            0, (void*) CUBE_VERTS_SIZE);
    glEnableVertexAttribArray(shader["texCoords"]);
    glVertexAttribPointer(shader["texCoords"], 2, GL_FLOAT, GL_FALSE,
            0, (void*) (CUBE_VERTS_SIZE + CUBE_NORMS_SIZE));

    GLuint spring_vao;
    GLuint spring_vbo;
    glGenVertexArrays(1, &spring_vao);
    glBindVertexArray(spring_vao);
    glGenBuffers(1, &spring_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, spring_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 2 * dimX * (dimY - 1), NULL, GL_STREAM_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint quad_vao;
    GLuint quad_vbo;
    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(XZ_quad_data), XZ_quad_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shader["normals"]);
    glVertexAttribPointer(shader["normals"], 3, GL_FLOAT, GL_FALSE,
            0, (void*) QUAD_VERTS_SIZE);
    glEnableVertexAttribArray(shader["texCoords"]);
    glVertexAttribPointer(shader["texCoords"], 2, GL_FLOAT, GL_FALSE,
            0, (void*) (QUAD_VERTS_SIZE + QUAD_NORMS_SIZE));

    GLuint texture = LoadTexture("textures/wood_floor.jpg");
    shader.AddUniform("tex");

    glLineWidth(LINE_WIDTH);
    // glEnable(GL_LINE_SMOOTH);

    bool quit = false;
    SDL_Event event;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    FPSCounter fpsC;
    fpsC.Init();
    fpsC.CallBack(print_lengths);
    while (!quit) {
        // Process all input events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                // key down events (wont repeat if holding key down)
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        camera.VelZ(1.0f);
                        break;
                    case SDLK_s:
                        camera.VelZ(-1.0f);
                        break;
                    case SDLK_a:
                        camera.VelX(-1.0f);
                        break;
                    case SDLK_d:
                        camera.VelX(1.0f);
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_p:
                        break;
                    case SDLK_SPACE:
                        break;
                    case SDLK_LEFT:
                        for (int c = 0; c < dimX; c++)
                            vel[dimY - 1][c].x -= 5;
                        break;
                    case SDLK_RIGHT:
                        for (int c = 0; c < dimX; c++)
                            vel[dimY - 1][c].x += 5;
                        break;
                    case SDLK_UP:
                        for (int c = 0; c < dimX; c++)
                            vel[dimY - 1][c].z -= 5;
                        break;
                    case SDLK_DOWN:
                        for (int c = 0; c < dimX; c++)
                            vel[dimY - 1][c].z += 5;
                        break;
                    case SDLK_r:
                        for (int r = 0; r < dimY; r++) {
                            for (int c = 0; c < dimX; c++) {
                                pos[r][c] = highp_dvec3(c*initDX, 5 - r*initDY, 0);
                                vel[r][c] = highp_dvec3(0,0,0);
                            }
                        }
                        break;
                }
            } else if (event.type == SDL_KEYUP) {
                // handle key up events
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_s:
                        camera.VelZ(0.0f);
                        break;
                    case SDLK_a:
                    case SDLK_d:
                        camera.VelX(0.0f);
                        break;
                    case SDLK_SPACE:
                        break;
                }
            } else if (event.type == SDL_MOUSEMOTION) {
                // handle mouse events
                float dx = event.motion.xrel;
                float dy = event.motion.yrel;
                camera.RotateX(-dy);
                camera.RotateY(-dx);
                camera.UpdateAxis();
            }
        }

        // update
        float t = SDL_GetTicks() / 1000.0f;
        fpsC.StartFrame(t);
        float dt = fpsC.GetDT();
        camera.Update(dt);

        for (int sims = 0; sims < 4; sims++) {
            highp_dvec3 accels[dimY][dimX] = { highp_dvec3(0, 0, 0) };

            for (int r = 1; r < dimY; ++r) {
                for (int c = 0; c < dimX; ++c) {
                    highp_dvec3 dpos = pos[r][c] - pos[r-1][c];
                    double stringLen = length(dpos);
                    highp_dvec3 dir = normalize(dpos);
                    double stringF = -KS*(stringLen - REST_LENGTH);
                    highp_dvec3 dampF = -KD*(vel[r][c] - vel[r-1][c]);
                    highp_dvec3 acc = highp_dvec3(0, 0, 0);
                    acc += stringF * dir + dampF +  GRAVITY * MASS;
                    acc *= 1.0/MASS;

                    accels[r][c] += acc;
                    accels[r-1][c] -= acc;
                }
            }

            for (int r = 1; r < dimY; ++r) {
                for (int c = 0; c < dimX; ++c) {
                    vel[r][c] += accels[r][c] * DT;
                    pos[r][c] += vel[r][c] * DT;
                }
            }
        }
        vec3 pArray[dimY][dimX];
        for (int r = 0; r < dimY; ++r) {
            for (int c = 0; c < dimX; ++c) {
                highp_dvec3 p = pos[r][c];
                pArray[r][c] = p;
            }
        }

        // draw
        glClearColor(1, 1, 1, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniformMatrix4fv(shader["VP"], 1,  GL_FALSE, value_ptr(camera.VP()));

        mat4 model(1);
        glBindVertexArray(quad_vao);
        model = translate(model, vec3(0, -10, 0));
        model = scale(model, vec3(50, 1, 50));
        glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(shader["tex"], 0);
        glUniform1i(shader["textured"], true);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUniform1i(shader["textured"], false);
        glBindVertexArray(cube_vao);
        for (int r = 0; r < dimY; ++r) {
            for (int c = 0; c < dimX; ++c) {
                model = mat4(1);
                model = translate(model, pArray[r][c]);
                model = scale(model, RADIUS * vec3(1, 1, 1));
                glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        int numLines = 2 * dimX * (dimY - 1);
        vec3 lineArray[numLines];
        int line = 0;
        for (int r = 0; r < dimY - 1; ++r) {
            for (int c = 0; c < dimX; ++c) {
                vec3 p1 = pArray[r][c];
                vec3 p2 = pArray[r+1][c];
                lineArray[line++] = p1;
                lineArray[line++] = p2;
            }
        }
        glUniform1i(shader["textured"], false);
        glBindVertexArray(spring_vao);
        model = mat4(1);
        glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
        glBindBuffer(GL_ARRAY_BUFFER, spring_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * numLines, lineArray, GL_STREAM_DRAW);
        glDrawArrays(GL_LINES, 0, numLines);

        fpsC.EndFrame(&pArray[0]);
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_Quit();

    return 0;
}
