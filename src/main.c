#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_UINT_DRAW_INDEX
#define NK_IMPLEMENTATION
#include <nuklear.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

typedef struct {
    unsigned char *image_data;
    int width;
    int height;
    int nchannels;
} Image;

typedef struct {
    GLuint ID;
    Image img;
} Texture;

typedef struct {
    float pos[2]; // important to keep it to 2 floats
    float uv[2];
    unsigned char col[4];
} Vertex;

typedef int Bool;

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

void texture_load_image(Texture *texture, const char *img_path) {
    stbi_set_flip_vertically_on_load(true);
    texture->img.image_data = stbi_load(img_path, &(texture->img.width), &(texture->img.height), &(texture->img.nchannels), 0);
}


void texture_create(Texture *texture, Bool gen_mipmap, ...) {
    va_list ptr;
    va_start(ptr, gen_mipmap);
    int filter = GL_LINEAR;
    if(gen_mipmap) {
        filter = va_arg(ptr, int);
    } 
    va_end(ptr);
    
    
    glGenTextures(1, &texture->ID);
    glBindTexture(GL_TEXTURE_2D, texture->ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int format;
    printf("%d\n", texture->img.nchannels);
    switch(texture->img.nchannels) {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, texture->img.width, texture->img.height, 0, format, GL_UNSIGNED_BYTE, texture->img.image_data);
    if(gen_mipmap) glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_image_free(Texture *texture) {
    stbi_image_free(texture->img.image_data);
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
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_DYNAMIC_DRAW);


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



    Texture tex;
    Image image;
    struct nk_font_atlas atlas;
    struct nk_draw_null_texture tex_null;
    struct nk_font *font;
    struct nk_context ctx;

    /*********************************************************************************
    NUKLEAR FONT BAKING
    **********************************************************************************/
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
    font = nk_font_atlas_add_from_file(&atlas, "res/unispace.ttf", 16, 0);
    image.image_data = (unsigned char*) nk_font_atlas_bake(&atlas, &image.width, &image.height, NK_FONT_ATLAS_RGBA32);
    image.nchannels = 4;
    tex.img = image;
    texture_create(&tex, FALSE, GL_LINEAR);
    nk_font_atlas_end(&atlas, nk_handle_id((int)tex.ID), &tex_null);
    glBindTexture(GL_TEXTURE_2D, tex.ID);

    /*********************************************************************************
    NUKLEAR CONTEXT INIT
    **********************************************************************************/
    if(!nk_init_default(&ctx, &font->handle)) {
        printf("Failed to initialize nk context!\n");
        nk_font_atlas_clear(&atlas);
        glfwTerminate();
        return 2;
    }

    
    /*********************************************************************************
    SETUP TO AQUIRE VERTEX AND ELEMENT BUFFER DATA FROM NUKLEAR
    **********************************************************************************/
    struct nk_convert_config cfg = {};
    static const struct nk_draw_vertex_layout_element vertex_layout[] = {
        {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex, pos)},
        {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex, uv)},
        {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(Vertex, col)},
        {NK_VERTEX_LAYOUT_END}
    };

    cfg.shape_AA = NK_ANTI_ALIASING_ON;
    cfg.line_AA = NK_ANTI_ALIASING_ON;
    cfg.vertex_layout = vertex_layout;
    cfg.vertex_size = sizeof(Vertex);
    cfg.vertex_alignment = NK_ALIGNOF(Vertex);
    cfg.circle_segment_count = 22;
    cfg.curve_segment_count = 22;
    cfg.arc_segment_count = 22;
    cfg.global_alpha = 1.0f;
    cfg.tex_null = tex_null;

    struct nk_buffer cmds, verts, idx;
    const struct nk_draw_command *cmd;
    nk_buffer_init_default(&cmds);
    nk_buffer_init_default(&verts);
    nk_buffer_init_default(&idx);
    nk_convert(&ctx, &cmds, &verts, &idx, &cfg);

    nk_draw_foreach(cmd, &ctx, &cmds) {
    if (!cmd->elem_count) continue;
        //[...]
    }

    nk_buffer_free(&cmds);
    nk_buffer_free(&verts);
    nk_buffer_free(&idx);

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


        double cursor_x, cursor_y;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        int left_button_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        nk_input_begin(&ctx);
        nk_input_motion(&ctx, (int) cursor_x, (int) cursor_y);
        if(left_button_state == GLFW_PRESS) {
            nk_input_button(&ctx, NK_BUTTON_LEFT, (int) cursor_x, (int) cursor_y, true);
        }
        nk_input_end(&ctx);


        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if(resolution[0] != width || resolution[1] != height) {
            glViewport(0,0,width,height);
            resolution[0] = width;
            resolution[1] = height;
        }

        nk_clear(&ctx);
    }

    
    nk_font_atlas_cleanup(&atlas);
    nk_font_atlas_clear(&atlas);
    nk_free(&ctx);
    glfwTerminate();
    return 0;
}
