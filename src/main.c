/* === src/main.c === Editor entry point */

#include <stdlib.h>

#include <SDL.h>

#include <goated/utils.h>
#include <goated/editor.h>
#include <goated/layout.h>

/* === PROTOTYPES === */

/* === PUBLIC FUNCTIONS === */

int main(int argc, const char *argv[]) {
    IGNORE(argc);
    IGNORE(argv);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        ERROR_FMT("failed to initialize SDL: %d", SDL_GetError());
    }
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    SDL_Window *win = SDL_CreateWindow("loved", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 700, 800, 0);

    const char *filename = "src/text.c";

    Editor state;
    editor_init(&state, win);
    editor_load_file(&state, filename);

    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN: {
                    char c = ev.key.keysym.sym;
                    editor_handle_cmd(&state, c);
                    break;
                }
                default:
                    continue;
            }
        }

        layout_frame(&state);
    }

    return EXIT_SUCCESS;
}

/* === PRIVATE FUNCTIONS === */
