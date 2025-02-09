CC= clang

SRC_FILES= $(wildcard src/*.c)
OBJ_FILES= $(patsubst src/%.c,obj/%.o,$(SRC_FILES))

TARGET= obj/goated

INCLUDES= -Iinc  -I/opt/homebrew/include/SDL2  -I/opt/homebrew/include/freetype2
CFLAGS= $(INCLUDES) -Wall -Wextra -Werror -std=c99 -g -D_THREAD_SAFE
LFLAGS= -L/opt/homebrew/lib -lSDL2 -lfreetype

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

obj/%.o: src/%.c | obj
	$(CC) -o $@ -c $^ $(CFLAGS)

obj:
	mkdir -p obj

run: $(TARGET)
	@./obj/goated

clean:
	rm -rf obj
