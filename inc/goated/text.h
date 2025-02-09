/* === inc/goated/text.h === Editable text buffer */

#ifndef GOATED_TEXT_H
#define GOATED_TEXT_H

#include <goated/utils.h>

typedef struct {
    char **lines;
    int len_lines, cap_lines;

    int *line_lens, *line_caps;
} TextBuf;

void text_buf_init_empty(TextBuf *buf);
bool text_buf_init_file(TextBuf *buf, const char *filename);

void text_buf_check_line(TextBuf *buf, int line);
void text_buf_set_line(TextBuf *buf, int line, const char *text, int text_len);
void text_buf_append_line(TextBuf *buf, const char *text, int text_len);

void text_buf_print(TextBuf *buf);

void text_buf_deinit(TextBuf *buf);

#endif /* GOATED_TEXT_H */
