/* === src/text.c === Editable text buffer */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <goated/text.h>
#include <goated/utils.h>

#define INIT_CAP_LINES 64
#define INIT_LINE_CAP 64

/* === PROTOTYPES === */

static int next_power_of_2(int lower_bound);

/* === PUBLIC FUNCTIONS === */

void text_buf_init_empty(TextBuf *buf) {
    buf->lines = calloc(INIT_CAP_LINES, sizeof(char *));
    buf->line_lens = calloc(INIT_CAP_LINES, sizeof(int));
    buf->line_caps = calloc(INIT_CAP_LINES, sizeof(int));

    buf->cap_lines = INIT_CAP_LINES;
    buf->len_lines = 0;

    for (int i = 0; i < INIT_CAP_LINES; i++) {
        buf->line_lens[i] = 0;
        buf->line_caps[i] = INIT_LINE_CAP;
        buf->lines[i] = calloc(1, INIT_LINE_CAP);
    }
}

bool text_buf_init_file(TextBuf *buf, const char *filename) {
    text_buf_init_empty(buf);

    FILE *f = fopen(filename, "rb");
    if (!f) {
        return false;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *text = calloc(1, len + 1);
    fread(text, 1, len, f);
    fclose(f);

    text[len] = 0;

    int mark = 0;
    while (text[mark] != 0) {
        int start = mark;
        while (text[mark] != '\n' && text[mark] != 0) {
            mark++;
        }

        int end = mark;
        int size = end - start;

        text_buf_append_line(buf, &text[start], size);

        mark++;
    }

    return true;
}

void text_buf_save(TextBuf *buf, const char *path) {
    FILE *f = fopen(path, "wb");

    for (int i = 0; i < buf->len_lines; i++) {
        fprintf(f, "%.*s\n", (int) buf->line_lens[i], buf->lines[i]);
    }

    fclose(f);
}

void text_buf_deinit(TextBuf *buf) {
    IGNORE(buf);
}

void text_buf_check_line_cap(TextBuf *buf, int line, int needed_cap) {
    if (buf->line_caps[line] < needed_cap) {
        int new_cap = next_power_of_2(needed_cap);
        char *new_line = calloc(new_cap, sizeof(char *));
        memcpy(new_line, buf->lines[line], 
                sizeof(char) * buf->line_lens[line]);
        free(buf->lines[line]);
        buf->lines[line] = new_line;
        buf->line_caps[line] = new_cap;
    }
}

void text_buf_check_line(TextBuf *buf, int line) {
    if (line >= buf->len_lines) {
        int new_cap_lines = next_power_of_2(buf->len_lines);

        char **new_lines = calloc(new_cap_lines, sizeof(char *)); 
        int *new_line_caps = calloc(new_cap_lines, sizeof(int)); 
        int *new_line_lens = calloc(new_cap_lines, sizeof(int)); 

        for (int i = 0; i < buf->len_lines; i++) {
            new_lines[i] = buf->lines[i];
            new_line_caps[i] = buf->line_caps[i];
            new_line_lens[i] = buf->line_lens[i];
        }

        for (int i = buf->len_lines; i < new_cap_lines; i++) {
            new_lines[i] = calloc(INIT_LINE_CAP, sizeof(char));
            new_line_caps[i] = INIT_LINE_CAP;
            new_line_lens[i] = 0;
        }

        free(buf->lines);
        free(buf->line_caps);
        free(buf->line_lens);

        buf->lines = new_lines;
        buf->line_caps = new_line_caps;
        buf->line_lens = new_line_lens;

        buf->cap_lines = new_cap_lines;
    }
}

void text_buf_set_line(TextBuf *buf, int line, const char *text, int text_len) {
    text_buf_check_line(buf, line);

    if (line >= buf->len_lines) {
        buf->len_lines = line + 1;
    }

    if (text_len >= buf->line_caps[line]) {
        buf->line_caps[line] = next_power_of_2(text_len);
        buf->lines[line] = realloc(buf->lines[line], buf->line_caps[line]);
    }

    buf->line_lens[line] = text_len;
    memcpy(buf->lines[line], text, text_len);
}

void text_buf_append_line(TextBuf *buf, const char *text, int text_len) {
    text_buf_set_line(buf, buf->len_lines, text, text_len);
}

void text_buf_insert_str(TextBuf *buf, int row, int col, const char *str, 
        int str_len) {
    text_buf_check_line_cap(buf, row, buf->line_lens[row] + str_len);

    for (int i = buf->line_lens[row] - 1; i >= col; i--) {
        buf->lines[row][i + str_len] = buf->lines[row][i];
    }

    for (int i = 0; i < str_len; i++) {
        buf->lines[row][col + i] = str[i];
    }
    buf->line_lens[row] += str_len;
}

void text_buf_insert(TextBuf *buf, int row, int col, int c) {
    char c_char = (char) c;
    text_buf_insert_str(buf, row, col, &c_char, 1);
}

void text_buf_delete(TextBuf *buf, int row, int col) {
    for (int i = col - 1; i < buf->line_lens[row] - 1; i++) {
        buf->lines[row][i] = buf->lines[row][i + 1];
    }

    buf->line_lens[row]--;
}

void text_buf_create_line(TextBuf *buf, int row) {
    text_buf_check_line(buf, buf->len_lines + 1);

    for (int i = buf->len_lines - 1; i > row + 1; i--) {
        buf->lines[i] = buf->lines[i - 1];
        buf->line_lens[i] = buf->line_lens[i - 1];
        buf->line_caps[i] = buf->line_caps[i - 1];
    }

    buf->lines[row + 1] = calloc(INIT_LINE_CAP, sizeof(char));
    buf->line_lens[row + 1] = 0;
    buf->line_caps[row + 1] = INIT_LINE_CAP;

    buf->len_lines++;
}

void text_buf_delete_line(TextBuf *buf, int row) {
    for (int i = row; i < buf->len_lines - 1; i++) {
        buf->lines[i] = buf->lines[i + 1];
        buf->line_lens[i] = buf->line_lens[i + 1];
        buf->line_caps[i] = buf->line_caps[i + 1];
    }

    buf->len_lines--;
}

void text_buf_print(TextBuf *buf) {
    for (int i = 0; i < buf->len_lines; i++) {
        printf("%.*s\n", buf->line_lens[i], buf->lines[i]);
    }
}

/* === PRIVATE FUNCTIONS === */

static int next_power_of_2(int lower_bound) {
    int num  = 2;
    while (num <= lower_bound) {
        num *= 2;
    }

    return num;
}
