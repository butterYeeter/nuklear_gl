#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_BOOL
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONK_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_UINT_DRAW_INDEX
#define NK_IMPLEMENTATION
#include "../include/nuklear.h"


// typedef struct {
//     GLuint id;
//     unsigned int *pnames;
//     unsigned int *params;
// } Texture;

int resolution[] = {800, 800};

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

void upload_uniform2i(const GLuint program, const char *name, const int v0, const int v1) {
    int location = glGetUniformLocation(program, name);
    glUseProgram(program);
    glUniform2i(program, v0, v1);

    int res[2];
    // glGetUniformiv(program, location, res);
    // printf("WIDTH: %dpx, HEIGHT: %dpx\n", res[0], res[1]);
}


void upload_uniform1f(const GLuint program, const char *name, const float v0) {
    glUseProgram(program);
    GLint location = glGetUniformLocation(program, name);
    glUniform1f(program, v0);

    float res;
    glGetUniformfv(program, location, &res);
    printf("val: %f\n", res);
}


int main(int argc, char **argv) {
    char cwd[512];
    getcwd(cwd, 256);
    char slash = '/';
    strncat(cwd, &slash, 1);
    strcat(cwd, argv[0]);
    printf("%s\n", cwd);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    GLFWwindow *window = glfwCreateWindow(resolution[0], resolution[1], "Hello World", NULL, NULL);
    assert(window != NULL);
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
    unsigned int format;
    (n == 4) ? format = GL_RGBA : (n==3) ? format = GL_RGB : (format);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);


    float last_time = glfwGetTime();
    while(!glfwWindowShouldClose(window)) {
        // upload_uniform2i(shader_program, "res", resolution[0], resolution[1]);
        glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        float time = glfwGetTime();
        int location = glGetUniformLocation(shader_program, "time");
        glUniform1f(location, time);
        // upload_uniform1f(shader_program, "time", dt);

        glfwSwapBuffers(window);
        glfwPollEvents();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if(resolution[0] != width || resolution[1] != height) {
            glViewport(0,0,width,height);
            resolution[0] = width;
            resolution[1] = height;
            // printf("resolution changed!\n");
        }
    }

    glfwTerminate();
    return 0;
}
