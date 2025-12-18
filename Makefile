# pamap = pathway mapper

.DEFAULT: normal
.PHONY:   normal clean debug depend

TARGET          = pamap
SRC_DIRS        = src src/parser
SRC_FILES       = $(foreach curr_dir, $(SRC_DIRS), $(wildcard ${curr_dir}/*.c))
OBJ_FILES       = $(patsubst %.c, build/%.o, $(notdir ${SRC_FILES}))
OBJ_FILES_DEBUG = $(patsubst %.c, build/%_debug.o, $(notdir ${SRC_FILES}))

CC       = gcc
CC_FLAGS = -g -Wall -Wconversion -Wextra -Wpedantic
LD_FLAGS = -g -lraylib -lm

normal: $(TARGET)

build:
	mkdir build

clean:
	rm $(OBJ_FILES) $(OBJ_FILES_DEBUG) $(TARGET) -f

debug: $(OBJ_FILES_DEBUG) | build
	@echo "building $@"
	@$(CC) -o $(TARGET) $^ $(LD_FLAGS) -fsanitize=address

$(TARGET): $(OBJ_FILES) | build
	@echo "building $@"
	@$(CC) -o $(TARGET) $^ $(LD_FLAGS)

VPATH = $(SRC_DIRS)
# normal build
build/%.o: %.c | build
	@echo "building $@"
	@$(CC) -o $@ -c $< $(CC_FLAGS)
# with debug flag on
build/%_debug.o: %.c | build
	@echo "building $@"
	@$(CC) -o $@ -c $< $(CC_FLAGS) -DDEBUG

# also gets rid of weird ../ backtrack for deeper source files
# is there a way to only have unique dependencies?
depend:
	@sed --in-place '/^# DEPENDENCY FILES$$/,$$d' Makefile
	@echo "# DEPENDENCY FILES"  >> Makefile
	@$(CC) -MM $(SRC_FILES) >> sed_temp.txt
	@sed --in-place 's#\(.*\):#build/\1:#' sed_temp.txt
	@sed --in-place "s#\(/[a-z_]\+/../\)\+#/#g" sed_temp.txt
	@cat sed_temp.txt >> Makefile
	@rm sed_temp.txt -f

# DEPENDENCY FILES
build/debug_funcs.o: src/debug_funcs.c src/includes/debug_funcs.h \
 src/includes/generic_array.h src/includes/structure.h \
 src/includes/tokenizer.h src/includes/types.h \
 src/includes/generic_array.h src/includes/types.h
build/graphics.o: src/graphics.c src/includes/structure.h \
 src/includes/generic_array.h src/includes/tokenizer.h \
 src/includes/types.h src/includes/graphics.h src/includes/structure.h
build/main.o: src/main.c src/includes/graphics.h src/includes/structure.h \
 src/includes/generic_array.h src/includes/tokenizer.h \
 src/includes/types.h src/includes/parser.h src/includes/structure.h
build/parser.o: src/parser.c src/includes/generic_array.h \
 src/includes/structure.h src/includes/generic_array.h \
 src/includes/tokenizer.h src/includes/types.h src/includes/parser.h \
 src/includes/structure.h
build/structure.o: src/structure.c src/includes/structure.h \
 src/includes/generic_array.h src/includes/tokenizer.h \
 src/includes/types.h
build/tokenizer.o: src/tokenizer.c src/includes/generic_array.h \
 src/includes/types.h src/includes/generic_array.h \
 src/includes/tokenizer.h src/includes/types.h
build/types.o: src/types.c src/includes/generic_array.h src/includes/types.h \
 src/includes/generic_array.h
