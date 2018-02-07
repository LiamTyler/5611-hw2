#include "include/utils.h"
#include "include/camera.h"
#include "include/glsl_shader.h"
#include "include/image.h"
#include "include/fps_counter.h"
#include "include/mesh.h"

using namespace std;

typedef struct Ball {
    Ball(vec3 p, vec3 v, float r) {
        pos = p;
        vel = v;
        radius = r;
    }
    vec3 pos;
    vec3 vel;
    float radius;
} Ball; 

#define RADIUS .2f
#define NUM_BALLS 10

int main(int arc, char** argv) {
    vec3 pos[NUM_BALLS];
    vec3 vel[NUM_BALLS];
    for (int i = 0; i < 10; i++) {
        pos[i] = vec3(0, 5 - i, 0);
        vel[i] = vec3(0,0,0);
    }

    // initialize SDL and GLEW and set up window
    SDL_Window* window = InitAndWindow("Starter Project", 100, 100, 800, 600);
    cout << "vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "version: " << glGetString(GL_VERSION) << endl;

    // set up the particle system
    Camera camera = Camera(vec3(0, 0, 5), vec3(0, 0, -1), vec3(0, 1, 0));
    GLSLShader shader;
    shader.LoadFromFile(GL_VERTEX_SHADER,   "shaders/plain_shader.vert");
    shader.LoadFromFile(GL_FRAGMENT_SHADER, "shaders/plain_shader.frag");
    shader.CreateAndLinkProgram();
    shader.Enable();
    shader.AddAttribute("verts");
    shader.AddUniform("model");
    shader.AddUniform("VP");
    static const GLfloat quad_verts[] = {
        -.5, .5, 0,
        -.5, -.5, 0,
        .5, -.5, 0,
        .5, -.5, 0,
        .5, .5, 0,
        -.5, .5, 0,
    };
    GLuint ball_vao;
    GLuint quad_vbo;
    glGenVertexArrays(1, &ball_vao);
    glBindVertexArray(ball_vao);
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLuint spring_vao;
    GLuint spring_vbo;
    glGenVertexArrays(1, &spring_vao);
    glBindVertexArray(spring_vao);
    glGenBuffers(1, &spring_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, spring_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * NUM_BALLS, pos, GL_STREAM_DRAW);
    glEnableVertexAttribArray(shader["verts"]);
    glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);

    // GLuint texture = LoadTexture("textures/circle.png");
    // shader.AddUniform("tex");

    bool quit = false;
    SDL_Event event;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    FPSCounter fpsC;
    fpsC.Init();
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

        // draw
        glClearColor(1, 1, 1, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(shader["VP"], 1,  GL_FALSE, value_ptr(camera.VP()));

        glBindVertexArray(ball_vao);
        for (int i = 0; i < NUM_BALLS; ++i) {
            mat4 model(1);
            model = translate(model, pos[i]);
            model = scale(model, RADIUS * vec3(1, 1, 1));
            glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(spring_vao);
        mat4 model(1);
        glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
        glBindBuffer(GL_ARRAY_BUFFER, spring_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*NUM_BALLS, pos, GL_STREAM_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0, NUM_BALLS);

        fpsC.EndFrame();
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_Quit();

    return 0;
}
