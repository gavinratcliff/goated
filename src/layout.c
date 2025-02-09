/* === src/layout.c === High-level buffer drawing */

#include <SDL.h>

#include <goated/layout.h>
#include <goated/render.h>

/* === PROTOTYPES === */

/* === PUBLIC FUNCTIONS === */

void layout_frame(Editor *ed) {
    SDL_SetRenderDrawColor(ed->render.renderer, 0, 0, 0, 255);
    SDL_RenderClear(ed->render.renderer);
    SDL_SetRenderDrawBlendMode(ed->render.renderer, SDL_BLENDMODE_BLEND);

    SDL_Color text_color = {255, 255, 255, 255};
    SDL_Color background_color = {0, 0, 0, 255};

    EditorBuf *buf = &ed->cur_buf;
    TextBuf *text = &buf->text;
    Font *monospace = ed->render.monospace;

    int line_start = ed->cur_buf.scroll;
    int line_end = MIN(ed->max_screen_lines + line_start, 
            text->len_lines - line_start);

    for (int i = line_start; i <= line_end; i++) {
        if (text->line_lens[i] == 0) {
            continue;
        }

        int x = X_START;
        int y = Y_START + monospace->max_font_height * i;
        render_text(&ed->render, monospace, text->lines[i], 
                text->line_lens[i], x, y, text_color);
    }

    SDL_Rect cursor_rect = {
        .x = X_START + buf->cursor_col * monospace->advance,
        .y = monospace->max_font_height * buf->cursor_row,
        .w = monospace->advance,
        .h = monospace->max_font_height,
    };

    render_rect(&ed->render, cursor_rect, text_color);

    int x = X_START + monospace->advance * buf->cursor_col;
    int y = Y_START + monospace->max_font_height * 
        (buf->cursor_row - ed->cur_buf.scroll);
    render_text(&ed->render, monospace, &text->lines[buf->cursor_row]
            [buf->cursor_col], 1, x, y, background_color);

    SDL_RenderPresent(ed->render.renderer);
}

/* === PRIVATE FUNCTIONS === */
