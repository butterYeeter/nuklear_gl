#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

// typedef struct {
//     GLuint id;
//     unsigned int *pnames;
//     unsigned int *params;
// } Texture;

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
    // glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 800, "Hello World", NULL, NULL);
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("FAILED TO LOAD GL FUNCTIONS!\n");
        return 1;
    }

    const unsigned char *version = glGetString(GL_VERSION);
    printf("GL VERSOIN: %s\n", version);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    float vertices[] = {
        -0.5f, -0.5f,   0.0f, 0.0f,
        0.5f, -0.5f,    1.0f, 0.0f,
        0.5f, 0.5f,     1.0f, 1.0f,
       -0.5f, 0.5f,   0.0f, 1.0f
    };

    GLuint indices[] = {
        0,1,2,  0,2,3
    };

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint shader_program, vert_shader, frag_shader;
    vert_shader = create_shader("res/vert.glsl", GL_VERTEX_SHADER);
    frag_shader = create_shader("res/frag.glsl", GL_FRAGMENT_SHADER);
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    GLint ret;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &ret);
    if(ret != GL_TRUE) {
        glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &ret);
        char log[ret];
        glGetProgramInfoLog(shader_program, ret, NULL, log);
        printf("%s\n", log);
        return 2;
    }

    glUseProgram(shader_program);


    int w, h, n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("res/dice.png", &w, &h, &n, 0);
    n = 2;
    unsigned int format;
    (n == 4) ? format = GL_RGBA : (n==3) ? format = GL_RGB : (format = 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);


    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
