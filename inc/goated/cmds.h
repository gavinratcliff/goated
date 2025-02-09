/* === inc/goated/cmds.h === Builtin editor command definitions */

#ifndef GOATED_CMDS_H
#define GOATED_CMDS_H

#include <goated/editor.h>

void cmd_move_up(Editor *ed);
void cmd_move_down(Editor *ed);
void cmd_move_left(Editor *ed);
void cmd_move_right(Editor *ed);

#endif /* GOATED_CMDS_H */
