/* === src/render.c === Software renderer */

#include <goated/render.h>
#include <goated/utils.h>

/* === PROTOTYPES === */

static Font *font_create(Renderer *render, const char *font_path, 
        int font_size);
static void font_destroy(Font *font);
static void draw_rect(Renderer *render, SDL_Rect rect, SDL_Color col);
static void draw_text(Renderer *render, Font *font, const char *text,
        int text_len, int x, int y, SDL_Color color);

/* === PUBLIC FUNCTIONS === */

void render_init(Renderer *render, SDL_Window *win) {
    render->win = win;

    render->renderer = SDL_CreateRenderer(win, -1, 
            SDL_RENDERER_ACCELERATED);

    int sw, sh;
    SDL_GetWindowSize(render->win, &render->sw, &render->sh);

    SDL_GetRendererOutputSize(render->renderer, &sw, &sh);

    if (FT_Init_FreeType(&render->ft)) {
        ERROR("could not initialize FreeType");
    }

    render->monospace = font_create(render, "assets/monospace.ttf", FONT_SIZE);
}

void render_deinit(Renderer *render) {
    font_destroy(render->monospace);

    FT_Done_FreeType(render->ft);

    SDL_DestroyRenderer(render->renderer);
}

void render_rect(Renderer *render, SDL_Rect rect, SDL_Color color) {
    draw_rect(render, rect, color);
}


void render_text(Renderer *render, Font *font, const char *text,
        int text_len, int x, int y, SDL_Color color) {
    draw_text(render, font, text, text_len, x, y, color);
}

/* === PRIVATE FUNCTIONS === */

static Font *font_create(Renderer *render, const char *font_path, 
        int font_size) {
    Font *font = calloc(1, sizeof(Font));

    if (FT_New_Face(render->ft, font_path, 0, &font->face)) {
        ERROR_FMT("failed to load font '%s'", font_path);
    }

    FT_Set_Pixel_Sizes(font->face, 0, font_size);
    SDL_Surface *surf = SDL_CreateRGBSurface(0, ATLAS_WIDTH, ATLAS_HEIGHT, 32,
                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    int x = 0, y = 0;
    unsigned int row_height = 0;
    unsigned int max_font_height = 0;
    int max_advance = 0;

    for (char c = START_CHAR; c <= END_CHAR; c++) {
        if (FT_Load_Char(font->face, c, 
                    FT_LOAD_RENDER  | FT_LOAD_TARGET_LIGHT)) {
            ERROR_FMT("failed to load character '%c'", c);
        }

        FT_Bitmap *bitmap = &font->face->glyph->bitmap;
        int advance = font->face->glyph->advance.x >> 6;
        if (advance > max_advance) {
            max_advance = advance;
        }

        if (x + bitmap->width >= ATLAS_WIDTH) {
            x = 0;
            y += row_height + 2;
            row_height = 0;
        }

        SDL_Rect dst_rect = {x, y, bitmap->width, bitmap->rows};
        font->glyphs[c - START_CHAR] = dst_rect;
        font->glyph_offsets[c - START_CHAR] = font->face->glyph->bitmap_top;

        for (unsigned int j = 0; j < bitmap->rows; j++) {
            for (unsigned int i = 0; i < bitmap->width; i++) {
                Uint8 pixel = bitmap->buffer[j * bitmap->width + i];
                Uint32 *pixels = (Uint32 *) surf->pixels;
                pixels[(y + j) * surf->w + (x + i)] = 
                    SDL_MapRGBA(surf->format, 255, 255, 255, pixel);
            }
        }

        x += bitmap->width + 2;

        if (bitmap->rows > row_height) {
            row_height = bitmap->rows;
        }

        if (bitmap->rows > max_font_height) {
            max_font_height = bitmap->rows;
        }
    }

    font->tex = SDL_CreateTextureFromSurface(render->renderer, surf);
    SDL_SetTextureBlendMode(font->tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(font->tex, SDL_ScaleModeLinear);

    font->max_font_height = (int) max_font_height;
    font->advance = (int) max_advance;

    SDL_FreeSurface(surf);

    return font;
}

void font_destroy(Font *font) {
    SDL_DestroyTexture(font->tex);

    free(font);
}

static void draw_rect(Renderer *render, SDL_Rect rect, SDL_Color color) {
    SDL_SetRenderDrawColor(render->renderer, color.r, color.g, color.b, 
            color.a);
    SDL_RenderFillRect(render->renderer, &rect);
}

static void draw_text(Renderer *render, Font *font, const char *text,
        int text_len, int x, int y, SDL_Color color) {
    SDL_SetTextureColorMod(font->tex, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(font->tex, color.a);

    int baseline = y;

    for (int pos = 0; pos < text_len; pos++) {
        int c = text[pos];
        if (c < START_CHAR || c > END_CHAR) continue;

        SDL_Rect src = font->glyphs[c - START_CHAR];
        int offset_y = font->glyph_offsets[c - START_CHAR];
        SDL_Rect dst = {x, baseline - offset_y, src.w, src.h};

        SDL_RenderCopy(render->renderer, font->tex, &src, &dst);

        x += font->advance;
    }

    SDL_SetTextureColorMod(font->tex, 255, 255, 255);
    SDL_SetTextureAlphaMod(font->tex, 255);
}
