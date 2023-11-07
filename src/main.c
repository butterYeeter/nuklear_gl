#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include <assert.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

GLuint create_shader(const char *shader_path, const unsigned int shader_type) {
    FILE *shader_file;
    char *shader_src;
    int file_size;

    shader_file = fopen(shader_path, "r");
    fseek(shader_file, 0, SEEK_END);
    file_size = ftell(shader_file);
    rewind(shader_file);
    shader_src = (char*) malloc(file_size);
    fread(shader_src, 1, file_size, shader_file);
    fclose(shader_file);

    GLuint shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_src, &file_size);
    glCompileShader(shader_id);

    GLint ret;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &ret);
    if(ret != GL_TRUE) {
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &ret);
        char log[ret];
        glGetShaderInfoLog(shader_id, ret, NULL, log);
        
        switch(shader_type) {
            case GL_VERTEX_SHADER:
                printf("================\nVertex Shader log:\n================\n%s\n", log);
                break;
            case GL_FRAGMENT_SHADER:
                printf("================\nFrag Shader log:\n================\n%s\n", log);
                break;
        }
    }

    free(shader_src);
    return shader_id;
}

int main() {
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(800, 800, "Hello World", NULL, NULL);
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("FAILED TO LOAD GL FUNCTIONS!\n");
        return 1;
    }

    float vertices[] = {
        -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
        0.0f, 0.37f,    0.0f, 1.0, 0.0f,
        0.5f, -0.5f,    0.0f, 0.0f, 1.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint program, vert_shader, frag_shader;
    vert_shader = create_shader("res/vert.glsl", GL_VERTEX_SHADER);
    frag_shader = create_shader("res/frag.glsl", GL_FRAGMENT_SHADER);
    program = glCreateProgram();
    glAttachShader(program, vert_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    GLint ret;
    glGetProgramiv(program, GL_LINK_STATUS, &ret);
    if(ret != GL_TRUE) {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);
        char log[ret];
        glGetProgramInfoLog(program, ret, NULL, log);
        printf("%s\n", log);
        return 2;
    }

    glUseProgram(program);


    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}