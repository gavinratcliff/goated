/* === inc/goated/editor.h === High-level editor state */

#ifndef GOATED_EDITOR_H
#define GOATED_EDITOR_H

#include <goated/text.h>
#include <goated/render.h>

#define Y_START 15
#define X_START 5

#define MAX_CMD_KEYS 128

typedef enum {
    MODE_NORMAL,

    _MODE_MAX,
} EditorMode;

typedef struct {
    TextBuf text;
    int cursor_row, cursor_col;
    const char *filename;
    int scroll;

    int last_column;
} EditorBuf;

typedef struct Editor Editor;
typedef void (*EditorCmdCallback)(Editor *ed);

typedef struct {
    EditorCmdCallback cbs[MAX_CMD_KEYS];
} CmdMap;

struct Editor {
    Renderer render;

    SDL_Window *win;
    EditorBuf cur_buf;

    EditorMode mode;

    int max_screen_lines;

    CmdMap cmds[_MODE_MAX];
};

void editor_init(Editor *ed, SDL_Window *win);

void editor_load_file(Editor *ed, const char *filename);

void editor_handle_cmd(Editor *ed, int key);
void editor_bind_cmd(Editor *ed, EditorMode mode, int key, 
        EditorCmdCallback cmd);

void cursor_set_row(Editor *ed, int row);
void cursor_set_col(Editor *ed, int col);

void editor_deinit(Editor *ed);

#endif /* GOATED_EDITOR_H */
