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
    SDL_Color dim_color = {128, 128, 128, 255};
    SDL_Color background_color = {0, 0, 0, 255};

    EditorBuf *buf = &ed->cur_buf;
    TextBuf *text = &buf->text;
    Font *monospace = ed->render.monospace;

    int line_start = ed->cur_buf.scroll;
    int line_end = MIN(ed->max_screen_lines + line_start, 
            text->len_lines - line_start);

    int real_x_start = X_START + monospace->advance * 3 + 5;

    for (int i = line_start; i <= line_end; i++) {
        int y = Y_START + monospace->max_font_height * 
            VISUAL_ROW(ed, i);

        int line_num = i;
        char line_num_str[3];
        if (line_num < 10) {
            line_num_str[0] = line_num_str[1] = ' ';
            line_num_str[2] = '0' + line_num;
        } else if (line_num < 100) {
            line_num_str[0] = ' ';
            line_num_str[1] = '0' + line_num / 10;
            line_num_str[2] = '0' + line_num % 10;
        } else if (line_num < 1000) {
            int first = line_num % 10;
            line_num /= 10;
            int second = line_num % 10;
            line_num_str[0] = line_num / 10;
            line_num_str[1] = second;
            line_num_str[2] = first;
        }

        render_text(&ed->render, monospace, line_num_str, 3, X_START, y, 
                dim_color);

        if (text->line_lens[i] == 0) {
            continue;
        }

        int x = real_x_start;
        render_text(&ed->render, monospace, text->lines[i], 
                text->line_lens[i], x, y, text_color);
        render_text(&ed->render, monospace, text->lines[i], 
                text->line_lens[i], x, y, text_color);
    }

    if (ed->mode == MODE_INSERT) {
        SDL_Rect cursor_rect = {
            .x = real_x_start + buf->cursor_col * monospace->advance,
            .y = monospace->max_font_height * VISUAL_ROW(ed, buf->cursor_row),
            .w = 2,
            .h = monospace->max_font_height,
        };

        render_rect(&ed->render, cursor_rect, text_color);
    } else {
        SDL_Rect cursor_rect = {
            .x = real_x_start + buf->cursor_col * monospace->advance,
            .y = monospace->max_font_height * VISUAL_ROW(ed, buf->cursor_row),
            .w = monospace->advance,
            .h = monospace->max_font_height,
        };

        render_rect(&ed->render, cursor_rect, text_color);

        int x = real_x_start + monospace->advance * buf->cursor_col;
        int y = Y_START + monospace->max_font_height * 
            (buf->cursor_row - ed->cur_buf.scroll);
        render_text(&ed->render, monospace, &text->lines[buf->cursor_row]
                [buf->cursor_col], 1, x, y, background_color);
    }


    SDL_RenderPresent(ed->render.renderer);
}

/* === PRIVATE FUNCTIONS === */
