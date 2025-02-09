/* === src/editor.c === High-level editor state */

#include <goated/editor.h>
#include <goated/cmds.h>

/* === PROTOTYPES === */

static void bind_default_cmds(Editor *ed);

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
    editor_bind_cmd(ed, MODE_NORMAL, 'j', cmd_move_up);
    editor_bind_cmd(ed, MODE_NORMAL, 'k', cmd_move_down);
}
