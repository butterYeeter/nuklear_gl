INCLUDE = -L./lib
LIBS = -lglad -lglfw3 -lm

bin/main: src/main.c
	gcc -o $@ $? $(INCLUDE) $(LIBS)
