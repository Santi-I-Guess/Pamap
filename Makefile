# pamap = pathway mapper
TARGET = pamap
SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(SRC_FILES:src/%.c=build/%.o)

CC = gcc
C_FLAGS = -g -Wpedantic -Wall
LD_FLAGS = -g

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) -o $(TARGET) $(OBJ_FILES) $(LD_FLAGS)

build/%.o: src/%.c | build
	$(CC) -o $@ -c $^ $(C_FLAGS)

build:
	mkdir -p build

clean:
	rm -f $(OBJ_FILES) $(TARGET)

depend:
	@sed --in-place '/^# dependency files$$/,$$d' Makefile
	@echo "# dependency files"  >> Makefile
	@$(CC) -MM $(SRC_FILES) | sed 's/\(.*\):/build\/\1:/' >> Makefile

.PHONY: clean debug

# dependency files
build/main.o: src/main.c
