#include "include/utils.h"
#include "include/camera.h"
#include "include/glsl_shader.h"
#include "include/image.h"
#include "include/fps_counter.h"
#include "include/mesh.h"
#include "include/shape_vertices.h"
#include "include/spring_system.h"

using namespace std;

bool HandleInput(SDL_Event& event, SpringSystem& ss, Camera& c);
void callback(void* data);

int main(int arc, char** argv) {
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

    GLuint texture = LoadTexture("textures/wood_floor.jpg");
    shader.AddUniform("tex");

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

    SpringSystem springSystem = SpringSystem(10, 10, 250, 20);
    springSystem.Setup(shader);

    glLineWidth(2);
    // glEnable(GL_LINE_SMOOTH);

    bool quit = false;
    SDL_Event event;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    FPSCounter fpsC;
    fpsC.Init();
    fpsC.CallBack(callback);
    while (!quit) {
        // Process all input events
        while (SDL_PollEvent(&event) && !quit) {
            quit = HandleInput(event, springSystem, camera);
		}

        // update
        float t = SDL_GetTicks() / 1000.0f;
        fpsC.StartFrame(t);
        float dt = fpsC.GetDT();
        camera.Update(dt);

        for (int i = 0; i < 1; i++) {
            springSystem.Update(0.001);
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

        springSystem.Render(shader);

        fpsC.EndFrame();
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_Quit();

    return 0;
}

bool HandleInput(SDL_Event& event, SpringSystem& ss, Camera& camera) {
    bool quit = false;
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
                break;
            case SDLK_RIGHT:
                break;
            case SDLK_UP:
                break;
            case SDLK_DOWN:
                break;
            case SDLK_r:
				ss.SpringSetup();
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
	return quit;
}

void callback(void* data) {
}
