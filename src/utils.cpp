#include "include/utils.h"
#include "include/image.h"

GLuint LoadTexture(string path) {
    Image image;
    if (!image.LoadImage(path)) {
        cout << "Failed to load tex: " << path << endl;
        return -1;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA,
            GL_FLOAT, image.Data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

SDL_Window* InitAndWindow(string title, int ox, int oy, int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "Failed to init SDL" << endl;
        return nullptr;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // specify the version of opengl you want
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_Window* window = SDL_CreateWindow(title.c_str(), ox, oy, w, h, SDL_WINDOW_OPENGL);
    if (window == NULL) {
        cout << "Failed to create an SDL2 window" << endl;
        exit(1);
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        cout << "Failed to create an opengl context" << endl;
        exit(1);
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "Failed to init GLEW" << endl;
        exit(1);
    }
    if (SDL_GL_SetSwapInterval(1) < 0)
        cout << "Failed to set vsync" << endl;

    glEnable(GL_DEPTH_TEST);

    return window;
}
