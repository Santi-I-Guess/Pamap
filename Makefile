# pamap = pathway mapper
TARGET    = pamap
SRC_DIRS  = src src/parser
SRC_FILES = $(foreach curr_dir, $(SRC_DIRS), $(wildcard $(curr_dir)/*.c))
OBJ_FILES = $(patsubst %.c, build/%.o, $(notdir $(SRC_FILES)))

CC = gcc
C_FLAGS = -g -Wpedantic -Wall
LD_FLAGS = -g

all: $(TARGET)

$(TARGET): $(OBJ_FILES) | build
	$(CC) -o $(TARGET) $(OBJ_FILES) $(LD_FLAGS)

VPATH = $(SRC_DIRS)
build/%.o: %.c | build
	$(CC) -o $@ -c $< $(C_FLAGS)

build:
	mkdir -p build

clean:
	rm $(OBJ_FILES) $(TARGET) -f

debug:
	$(CC) -o $(TARGET) -DDEBUG $(SRC_FILES) $(CFLAGS)

# also gets rid of weird /../ backtrack for deeper source files
depend:
	@sed --in-place=.bak '/^# dependency files$$/,$$d' Makefile
	@echo "# dependency files"  >> Makefile
	@$(CC) -MM $(SRC_FILES) >> sed.txt
	@sed --in-place --e 's#\(.*\):#build/\1:#' -e "s#/[a-z]\+/../#/#g" sed.txt
	@cat sed.txt >> Makefile
	@rm sed.txt -f

.PHONY: clean debug
# dependency files
build/main.o: src/main.c src/parser/tokenizer.h src/generic_array.h \
 src/types.h src/generic_array.h src/types.h
build/types.o: src/types.c src/generic_array.h src/types.h
build/parser.o: src/parser/parser.c src/generic_array.h \
 src/structure.h src/generic_array.h \
 src/parser/tokenizer.h src/parser/../generic_array.h \
 src/parser/../types.h src/parser/../generic_array.h \
 src/parser/parser.h
build/tokenizer.o: src/parser/tokenizer.c src/generic_array.h \
 src/types.h src/generic_array.h \
 src/parser/tokenizer.h
