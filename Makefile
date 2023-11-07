INCLUDE = -L./lib
LIBS = -lglad -lglfw3 -lcglm -lm

bin/main: src/main.c
	gcc -o $@ $? $(INCLUDE) $(LIBS)
