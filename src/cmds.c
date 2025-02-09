/* === src/cmds.c === Builtin editor command implementations */

#include <goated/cmds.h>

/* === PROTOTYPES === */

/* === PUBLIC FUNCTIONS === */

void cmd_move_up(Editor *ed) {
    if (ed->cur_buf.cursor_row + 1 < ed->cur_buf.text.len_lines) {
        cursor_set_row(ed, ed->cur_buf.cursor_row + 1);
    }
}

void cmd_move_down(Editor *ed) {
    if (ed->cur_buf.cursor_row > 0) {
        cursor_set_row(ed, ed->cur_buf.cursor_row - 1);
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

/* === PRIVATE FUNCTIONS === */
