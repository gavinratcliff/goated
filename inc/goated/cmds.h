/* === inc/goated/cmds.h === Builtin editor command definitions */

#ifndef GOATED_CMDS_H
#define GOATED_CMDS_H

#include <goated/editor.h>

void cmd_move_up(Editor *ed);
void cmd_move_down(Editor *ed);
void cmd_move_left(Editor *ed);
void cmd_move_right(Editor *ed);

void cmd_move_next_word(Editor *ed);
void cmd_move_prev_word(Editor *ed);

void cmd_jump_newline(Editor *ed);

void cmd_save_buffer(Editor *ed);

void cmd_switch_to_insert_left(Editor *ed);
void cmd_switch_to_insert_right(Editor *ed);
void cmd_switch_to_normal(Editor *ed);
void cmd_switch_to_meta(Editor *ed);

#endif /* GOATED_CMDS_H */
