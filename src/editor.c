/* === src/editor.c === High-level editor state */

#include <goated/editor.h>
#include <goated/cmds.h>

/* === PROTOTYPES === */

static void bind_default_cmds(Editor *ed);
static bool is_insert_char(Editor *ed, int key);

/* === PUBLIC FUNCTIONS === */

void editor_init(Editor *ed, SDL_Window *win) {
    ed->win = win;
    render_init(&ed->render, win);

    ed->max_screen_lines = (ed->render.sh - Y_START) / 
        ed->render.monospace->max_font_height;

    ed->mode = MODE_NORMAL;

    for (int mode = 0; mode < _MODE_MAX; mode++) {
        for (int key = 0; key < MAX_CMD_KEYS; key++) {
            ed->cmds[mode].cbs[key] = NULL;
        }
    }

    bind_default_cmds(ed);
}

void editor_load_file(Editor *ed, const char *filename) {
    if (!text_buf_init_file(&ed->cur_buf.text, filename)) {
        ERROR_FMT("failed to load file '%s'", filename);
    }

    ed->cur_buf.filename = filename;
    ed->cur_buf.cursor_row = 0;
    ed->cur_buf.cursor_col = 0;
    ed->cur_buf.scroll = 0;

    ed->cur_buf.last_column = 0;
}

void editor_handle_cmd(Editor *ed, int key) {
    CmdMap *map = &ed->cmds[ed->mode];
    if (key < 0 || key >= MAX_CMD_KEYS) {
        return;
    }

    if (ed->mode == MODE_INSERT) {
        if (key == 13) {
            text_buf_create_line(&ed->cur_buf.text, ed->cur_buf.cursor_row);

            int old_row = ed->cur_buf.cursor_row;
            int new_row = old_row + 1;
            char *from = &ed->cur_buf.text.lines[old_row][ed->cur_buf.cursor_col];
            int amt_left = ed->cur_buf.text.line_lens[old_row] 
                - ed->cur_buf.cursor_col;
            text_buf_insert_str(&ed->cur_buf.text, new_row, 0, from, amt_left);

            ed->cur_buf.text.line_lens[old_row] = ed->cur_buf.cursor_col;
            cursor_set_col(ed, 0);
            cursor_set_row(ed, ed->cur_buf.cursor_row+1);
        } else if (key == 8) {
            if (ed->cur_buf.cursor_col == 0) {
                if (ed->cur_buf.cursor_row > 0) {
                    int len = ed->cur_buf.text.line_lens[ed->cur_buf.cursor_row - 1];
                    text_buf_insert_str(&ed->cur_buf.text, ed->cur_buf.cursor_row - 1,
                            ed->cur_buf.text.line_lens[ed->cur_buf.cursor_row - 1], 
                            ed->cur_buf.text.lines[ed->cur_buf.cursor_row],
                            ed->cur_buf.text.line_lens[ed->cur_buf.cursor_row]);
                    text_buf_delete_line(&ed->cur_buf.text, ed->cur_buf.cursor_row);
                    cursor_set_row(ed, ed->cur_buf.cursor_row - 1);
                    cursor_set_col(ed, len);
                }
            } else {
                text_buf_delete(&ed->cur_buf.text, ed->cur_buf.cursor_row,
                        ed->cur_buf.cursor_col);
                cursor_set_col(ed, ed->cur_buf.cursor_col - 1);
            }
        }

        if (is_insert_char(ed, key)) {
            text_buf_insert(&ed->cur_buf.text, ed->cur_buf.cursor_row, 
                    ed->cur_buf.cursor_col, key);
            cursor_set_col(ed, ed->cur_buf.cursor_col + 1);
            return;
        }
    }

    if (map->cbs[key]) {
        map->cbs[key](ed);
    }

}

void editor_bind_cmd(Editor *ed, EditorMode mode, int key, 
        EditorCmdCallback cmd) {
    CmdMap *map = &ed->cmds[mode];
    map->cbs[key] = cmd;
}

void editor_deinit(Editor *ed) {
    IGNORE(ed);
}

void cursor_set_row(Editor *ed, int row) {
    ed->cur_buf.cursor_row = row;

    int len = ed->cur_buf.text.line_lens[ed->cur_buf.cursor_row] ;
    if (len == 0) {
        ed->cur_buf.cursor_col = 
            MIN(0, ed->cur_buf.last_column);
    } else {
        ed->cur_buf.cursor_col = 
            MIN(len - 1, ed->cur_buf.last_column);
    }
}

void cursor_set_col(Editor *ed, int col) {
    ed->cur_buf.cursor_col = col;
    ed->cur_buf.last_column = col;
}

/* === PRIVATE FUNCTIONS === */

static void bind_default_cmds(Editor *ed) {
    editor_bind_cmd(ed, MODE_NORMAL, 'h', cmd_move_left);
    editor_bind_cmd(ed, MODE_NORMAL, 'l', cmd_move_right);
    editor_bind_cmd(ed, MODE_NORMAL, 'j', cmd_move_down);
    editor_bind_cmd(ed, MODE_NORMAL, 'k', cmd_move_up);

    editor_bind_cmd(ed, MODE_NORMAL, 'w', cmd_move_next_word);
    editor_bind_cmd(ed, MODE_NORMAL, 'b', cmd_move_prev_word);

    editor_bind_cmd(ed, MODE_NORMAL, 'o', cmd_jump_newline);

    editor_bind_cmd(ed, MODE_NORMAL, 'i', cmd_switch_to_insert_left);
    editor_bind_cmd(ed, MODE_NORMAL, 'a', cmd_switch_to_insert_right);
    editor_bind_cmd(ed, MODE_NORMAL, ' ', cmd_switch_to_meta);
    editor_bind_cmd(ed, MODE_INSERT, 27, cmd_switch_to_normal);
    editor_bind_cmd(ed, MODE_META, 27, cmd_switch_to_normal);

    editor_bind_cmd(ed, MODE_META, 's', cmd_save_buffer);
}


static bool is_insert_char(Editor *ed, int key) {
    IGNORE(ed);

    return (key >= 33 && key <= 126) || key == ' ';
}
