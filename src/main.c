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

    SDL_Window *win = SDL_CreateWindow("goated", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 700, 800, 0);

    const char *filename = "test.c";

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
                    SDL_Keycode key = ev.key.keysym.sym;
                    SDL_Keymod mod = SDL_GetModState();

                    char c = (char)key;
                    if ((mod & KMOD_SHIFT) && key >= SDLK_a && key <= SDLK_z) {
                        c = (char)(key - 32); // Convert to uppercase
                    }

                    if (mod & KMOD_SHIFT) {
                        switch (key) {
                            case SDLK_1: c = '!'; break;
                            case SDLK_2: c = '@'; break;
                            case SDLK_3: c = '#'; break;
                            case SDLK_4: c = '$'; break;
                            case SDLK_5: c = '%'; break;
                            case SDLK_6: c = '^'; break;
                            case SDLK_7: c = '&'; break;
                            case SDLK_8: c = '*'; break;
                            case SDLK_9: c = '('; break;
                            case SDLK_0: c = ')'; break;
                            case SDLK_MINUS: c = '_'; break;
                            case SDLK_EQUALS: c = '+'; break;
                            case SDLK_LEFTBRACKET: c = '{'; break;
                            case SDLK_RIGHTBRACKET: c = '}'; break;
                            case SDLK_BACKSLASH: c = '|'; break;
                            case SDLK_SEMICOLON: c = ':'; break;
                            case SDLK_QUOTE: c = '"'; break;
                            case SDLK_COMMA: c = '<'; break;
                            case SDLK_PERIOD: c = '>'; break;
                            case SDLK_SLASH: c = '?'; break;
                        }
                    }

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
