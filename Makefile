RPATH = -Wl,-rpath,$(ORIGIN)/../lib
INCLUDE = -L./lib
LIBS = -lglad -lglfw3 -lcglm

build/main: src/main.c
	gcc -o $@ $? $(RPATH) $(INCLUDE) $(LIBS)
