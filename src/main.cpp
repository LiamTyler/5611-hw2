#include "include/utils.h"
#include "include/camera.h"
#include "include/glsl_shader.h"
#include "include/image.h"
#include "include/fps_counter.h"
#include "include/mesh.h"
#include "include/shape_vertices.h"
#include "include/spring_system.h"
#include "include/sphere.h"

using namespace std;

bool HandleInput(SDL_Event& event, float dt, SpringSystem& ss, Camera& c, Sphere& sphere);
void callback(void* data);

int main(int argc, char** argv) {
	int start_rows = 10;
	int start_cols = 10;
	int start_ks = 500;
	int start_kd = 100;
	if (argc > 1) {
		start_rows = stoi(argv[1]);
		if (argc > 2) {
			start_cols = stoi(argv[2]);
			if (argc > 3) {
				start_ks = stoi(argv[3]);
				if (argc > 4) {
					start_kd = stoi(argv[4]);
				}
			}
		}
	}
	for (int i = 0; i < argc; i++) {
		cout << "argv[" << i << "]: " << argv[i] << endl;
	}
    // initialize SDL and GLEW and set up window
    SDL_Window* window = InitAndWindow("Cloth Simulation", 100, 100, 800, 600);
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
    shader.AddUniform("normalMatrix");
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

	Mesh* sphereMesh = new Mesh;
	sphereMesh->LoadMesh("models/sphere.obj");
	GLuint sphere_vao;
	GLuint sphere_vbos[3];
	glGenVertexArrays(1, &sphere_vao);
	glBindVertexArray(sphere_vao);
	glGenBuffers(3, sphere_vbos);
	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sphereMesh->GetNumVertices() * sizeof(vec3),
			sphereMesh->GetVertices(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(shader["verts"]);
	glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, sphere_vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sphereMesh->GetNumVertices() * sizeof(vec3),
			sphereMesh->GetNormals(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(shader["normals"]);
	glVertexAttribPointer(shader["normals"], 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_vbos[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereMesh->GetNumTriangles() * sizeof(ivec3),
			sphereMesh->GetIndices(), GL_STATIC_DRAW);

	/*
	GLuint cube_vao;
	GLuint cube_vbos[1];
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);
	glGenBuffers(1, cube_vbos);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, CUBE_VERTS_SIZE + CUBE_NORMS_SIZE, cube_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(shader["verts"]);
	glVertexAttribPointer(shader["verts"], 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader["normals"]);
	glVertexAttribPointer(shader["normals"], 3, GL_FLOAT, GL_FALSE, 0, (void*)CUBE_VERTS_SIZE);
	*/

	Sphere sphere(glm::vec3(2.5, 2.5, 2.5), 1);

    SpringSystem springSystem = SpringSystem(start_rows, start_cols, start_ks, start_kd);
	springSystem.Setup();


    bool quit = false;
    SDL_Event event;
    SDL_SetRelativeMouseMode(SDL_TRUE);
    FPSCounter fpsC;
    fpsC.Init();
    fpsC.CallBack(callback);
    while (!quit) {
        float t = SDL_GetTicks() / 1000.0f;
        fpsC.StartFrame(t);
        float dt = fpsC.GetDT();

        // Process all input events
        while (SDL_PollEvent(&event) && !quit) {
            quit = HandleInput(event, dt, springSystem, camera, sphere);
		}

        // update
        camera.Update(dt);
		sphere.Update(dt);

        for (int i = 0; i < 15; i++) {
            springSystem.Update(0.0001);
			springSystem.HandleCollisions(sphere);
        }


        // draw
        glClearColor(1, 1, 1, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Enable();
        glUniformMatrix4fv(shader["VP"], 1,  GL_FALSE, value_ptr(camera.VP()));
        mat4 model(1);
        glBindVertexArray(quad_vao);
        model = translate(model, vec3(0, -10, 0));
        model = scale(model, vec3(50, 1, 50));
        glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
		mat4 nM = transpose(inverse(camera.View() * model));
        glUniformMatrix4fv(shader["normalMatrix"], 1,  GL_FALSE, value_ptr(nM));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(shader["tex"], 0);
        glUniform1i(shader["textured"], true);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(sphere_vao);
		model = sphere.GetModelMatrix();
        glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
		nM = transpose(inverse(camera.View() * model));
        glUniformMatrix4fv(shader["normalMatrix"], 1,  GL_FALSE, value_ptr(nM));
        glUniform1i(shader["textured"], false);
        glDrawElements(GL_TRIANGLES, sphereMesh->GetNumTriangles() * 3, GL_UNSIGNED_INT, 0);

		/*
		glBindVertexArray(cube_vao);
		model = sphere.GetModelMatrix();
        glUniformMatrix4fv(shader["model"], 1, GL_FALSE, value_ptr(model));
        glUniform1i(shader["textured"], false);
        glDrawArrays(GL_TRIANGLES, 0, 36);
		*/

        springSystem.Render(camera.View(), camera.Proj());

        fpsC.EndFrame();
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_Quit();

    return 0;
}

bool HandleInput(SDL_Event& event, float dt, SpringSystem& ss, Camera& camera, Sphere& sphere) {
    bool quit = false;
    if (event.type == SDL_QUIT) {
        quit = true;
    } else if (event.type == SDL_KEYDOWN) { // && event.key.repeat == 0) {
		bool print = false;
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
            case SDLK_i:
				{
				// increase KS
				double k = ss.GetKS();
				ss.SetKS(k + 10);
				}
				print = true;
                break;
            case SDLK_k:
				{
				// decrease KS
				double k = std::fmax(1, ss.GetKS() - 10);
				ss.SetKS(k);
				}
				print = true;
                break;
            case SDLK_l:
				{
				// increase KD
				double k = ss.GetKD() + 10;
				ss.SetKD(k + 10);
				}
				print = true;
                break;
            case SDLK_j:
				{
				// decrease KD
				double k = std::fmax(1, ss.GetKD() - 10);
				ss.SetKD(k);
				}
				print = true;
                break;
            case SDLK_ESCAPE:
                quit = true;
                break;
            case SDLK_p:
				ss.Pause();
                break;
            case SDLK_SPACE:
				ss.Drag(!ss.Drag());
				if (ss.Drag())
					cout << "Drag is on" << endl;
				else
					cout << "Drag is off" << endl;
                break;
            case SDLK_LEFT:
				sphere.velocity.x = -1;
                break;
            case SDLK_RIGHT:
				sphere.velocity.x = 1;
                break;
            case SDLK_UP:
				sphere.velocity.z = -1;
                break;
            case SDLK_DOWN:
				sphere.velocity.z = 1;
                break;
            case SDLK_v:
				ss.SpringSetup(true);
                break;
            case SDLK_x:
				ss.stuck = !ss.stuck;
                break;
            case SDLK_h:
				ss.SpringSetup(false);
                break;
            case SDLK_c:
				ss.ChangeVizualization();
                break;
        }
		if (print) {
			cout << "KS: " << ss.GetKS() << endl;
			cout << "KD: " << ss.GetKD() << endl;
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
			case SDLK_LEFT:
				sphere.velocity.x = 0;
				break;
			case SDLK_RIGHT:
				sphere.velocity.x = 0;
				break;
			case SDLK_UP:
				sphere.velocity.z = 0;
				break;
			case SDLK_DOWN:
				sphere.velocity.z = 0;
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
