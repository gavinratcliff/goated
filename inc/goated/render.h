/* === inc/goated/render.h === Software renderer */

#ifndef GOATED_RENDER_H
#define GOATED_RENDER_H

#include <SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define FONT_SIZE 14

#define ATLAS_WIDTH 512
#define ATLAS_HEIGHT 512
#define START_CHAR 32
#define END_CHAR 126

typedef struct {
    SDL_Texture *tex;
    SDL_Rect glyphs[END_CHAR - START_CHAR + 1];
    int glyph_offsets[END_CHAR - START_CHAR + 1];
    FT_Face face;
    int advance;
    int max_font_height;
} Font;

typedef struct {
    SDL_Window *win;
    SDL_Renderer *renderer;

    int sw, sh;

    Font *monospace;

    FT_Library ft;
} Renderer;

void render_init(Renderer *render, SDL_Window *win);

void render_deinit(Renderer *render);

void render_rect(Renderer *render, SDL_Rect rect, SDL_Color color);
void render_text(Renderer *render, Font *font, const char *text,
        int text_len, int x, int y, SDL_Color color);

#endif /* GOATED_RENDER_H */
