INCLUDE = -L./lib
LIBS = -lglad -l:libglfw3.a -lm

bin/main: src/main.c
	gcc -o $@ $? $(INCLUDE) $(LIBS) $(DEBUG)

bin/parser: src/fnt_parser.c
	gcc -o $@ $?