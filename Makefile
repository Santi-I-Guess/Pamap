# pamap = pathway mapper

.DEFAULT: normal
.PHONY: normal clean debug depend

TARGET          = pamap
SRC_DIRS        = src src/parser
SRC_FILES       = $(foreach curr_dir, $(SRC_DIRS), $(wildcard ${curr_dir}/*.c))
OBJ_FILES       = $(patsubst %.c, build/%.o, $(notdir ${SRC_FILES}))
OBJ_FILES_DEBUG = $(patsubst %.c, build/debug_%.o, $(notdir ${SRC_FILES}))

CC        = gcc
CC_FLAGS = -g -Wall -Wconversion -Wextra -Wpedantic
LD_FLAGS = -g -lraylib -lm

normal: $(TARGET)

build:
	mkdir -p build

clean:
	rm $(OBJ_FILES) $(TARGET) -f

debug: $(OBJ_FILES_DEBUG) | build
	$(CC) -o $(TARGET) $^ $(LD_FLAGS)

$(TARGET): $(OBJ_FILES) | build
	$(CC) -o $(TARGET) $^ $(LD_FLAGS)

VPATH = $(SRC_DIRS)
# normal build
build/%.o: %.c | build
	$(CC) -o $@ -c $< $(CC_FLAGS)
# with debug flag on
build/debug_%.o: %.c | build
	$(CC) -o $@ -c $< $(CC_FLAGS) -DDEBUG

# also gets rid of weird ../ backtrack for deeper source files
# is there a way to only have unique dependencies?
depend:
	sed --in-place '/^# DEPENDENCY FILES$$/,$$d' Makefile
	echo "# DEPENDENCY FILES"  >> Makefile
	$(CC) -MM $(SRC_FILES) >> sed_temp.txt
	sed --in-place 's#\(.*\):#build/\1:#' sed_temp.txt
	sed --in-place "s#\(/[a-z_]\+/../\)\+#/#g" sed_temp.txt
	cat sed_temp.txt >> Makefile
	rm sed_temp.txt -f

# DEPENDENCY FILES
build/debug_funcs.o: src/debug_funcs.c src/debug_funcs.h src/generic_array.h \
 src/structure.h src/parser/tokenizer.h src/generic_array.h \
 src/types.h src/generic_array.h src/types.h
build/graphics.o: src/graphics.c src/structure.h src/generic_array.h \
 src/parser/tokenizer.h src/generic_array.h \
 src/types.h src/generic_array.h
build/main.o: src/main.c src/graphics.h src/structure.h src/generic_array.h \
 src/parser/tokenizer.h src/generic_array.h \
 src/types.h src/generic_array.h src/parser/parser.h \
 src/structure.h
build/structure.o: src/structure.c src/structure.h src/generic_array.h \
 src/parser/tokenizer.h src/generic_array.h \
 src/types.h src/generic_array.h
build/types.o: src/types.c src/generic_array.h src/types.h
build/parser.o: src/parser/parser.c src/generic_array.h \
 src/structure.h src/generic_array.h \
 src/parser/tokenizer.h src/parser/../generic_array.h \
 src/parser/../types.h src/parser/../generic_array.h \
 src/parser/parser.h
build/tokenizer.o: src/parser/tokenizer.c src/generic_array.h \
 src/types.h src/generic_array.h \
 src/parser/tokenizer.h
