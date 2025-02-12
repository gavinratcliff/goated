/* === src/cmds.c === Builtin editor command implementations */

#include <goated/cmds.h>

/* === PROTOTYPES === */

static bool is_skip_punct(char c);

/* === PUBLIC FUNCTIONS === */

void cmd_move_up(Editor *ed) {
    if (ed->cur_buf.cursor_row > 0) {
        cursor_set_row(ed, ed->cur_buf.cursor_row - 1);
    }
}

void cmd_move_down(Editor *ed) {
    if (ed->cur_buf.cursor_row + 1 < ed->cur_buf.text.len_lines) {
        if (VISUAL_ROW(ed, ed->cur_buf.cursor_row) >= ed->max_screen_lines){
            ed->cur_buf.scroll++;
        }
        cursor_set_row(ed, ed->cur_buf.cursor_row + 1);
    }
}

void cmd_move_left(Editor *ed) {
    if (ed->cur_buf.cursor_col > 0) {
        cursor_set_col(ed, ed->cur_buf.cursor_col - 1);
    }
}

void cmd_move_right(Editor *ed) {
    if (ed->cur_buf.cursor_col + 1 < 
            ed->cur_buf.text.line_lens[ed->cur_buf.cursor_row]) {
        cursor_set_col(ed, ed->cur_buf.cursor_col + 1);
    }
}

void cmd_move_next_word(Editor *ed) {
    int row = ed->cur_buf.cursor_row;
    int col = ed->cur_buf.cursor_col;

    const char *line = ed->cur_buf.text.lines[row];
    char first_char = line[col];
    bool found_space = isspace(first_char);

    if (is_skip_punct(first_char)) {
        for (int i = col + 1; i < ed->cur_buf.text.line_lens[row]; i++) {
            if (!isspace(line[i]) && (line[i] != first_char)) {
                cursor_set_col(ed, i);
                return;
            }
        }
    } else {
        for (int i = col; i < ed->cur_buf.text.line_lens[row]; i++) {
            if (isspace(line[i])) {
                found_space = true;
            }

            if (is_skip_punct(line[i]) || (found_space && !isspace(line[i]))) {
                cursor_set_col(ed, i);
                return;
            }
        }
    }

    cursor_set_col(ed, 0);
    if (row + 1 < ed->cur_buf.text.len_lines) {
        cursor_set_row(ed, row + 1);
    }
}

void cmd_move_prev_word(Editor *ed) {
    int row = ed->cur_buf.cursor_row;
    int col = ed->cur_buf.cursor_col;

    const char *line = ed->cur_buf.text.lines[row];
    char first_char = line[col];
    bool found_space = isspace(first_char);

    if (is_skip_punct(first_char)) {
        for (int i = col; i >= 0; i--) {
            if (!isspace(line[i]) && (line[i] != first_char)) {
                cursor_set_col(ed, i);
                return;
            }
        }
    } else {
        for (int i = col; i >= 0; i--) {
            if (isspace(line[i])) {
                found_space = true;
            }

            if (is_skip_punct(line[i]) || (found_space && !isspace(line[i]))) {
                cursor_set_col(ed, i);
                return;
            }
        }
    }

    cursor_set_col(ed, 0);

    if (row > 0) {
        cursor_set_row(ed, row - 1);
    }
}

void cmd_jump_newline(Editor *ed) {
    text_buf_create_line(&ed->cur_buf.text, ed->cur_buf.cursor_row);

    cursor_set_col(ed, 0);

    cursor_set_row(ed, ed->cur_buf.cursor_row + 1);
    ed->mode = MODE_INSERT;
    ed->insert_right = false;
}

void cmd_save_buffer(Editor *ed) {
    text_buf_save(&ed->cur_buf.text, ed->cur_buf.filename);
    ed->mode = MODE_NORMAL;
}

void cmd_switch_to_insert_right(Editor *ed) {
    cursor_set_col(ed, ed->cur_buf.cursor_col + 1);
    ed->mode = MODE_INSERT;
    ed->insert_right = true;
}

void cmd_switch_to_insert_left(Editor *ed) {
    ed->mode = MODE_INSERT;
    ed->insert_right = false;
}

void cmd_switch_to_normal(Editor *ed) {
    if (ed->cur_buf.cursor_col > 0) {
        cursor_set_col(ed, ed->cur_buf.cursor_col - 1);
    }
    ed->mode = MODE_NORMAL;
}

void cmd_switch_to_meta(Editor *ed) {
    ed->mode = MODE_META;
}

/* === PRIVATE FUNCTIONS === */

static bool is_skip_punct(char c) {
    return ispunct(c) && c != '_';
}
