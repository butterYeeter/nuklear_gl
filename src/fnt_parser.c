#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UV {
    float x;
    float y;
};

struct Character{
   struct UV u  v;
   int32_t width;
   int32_t height;
   int32_t xadvance;
   int32_t x_offset;
   int32_t y_offset;
};

#define FONT_IMAGE_SCALE 512

typedef struct Character *CharacterSet;
static uint32_t file_offset;
static FILE *fnt_file;

void load_font_file(const char *path_to_fnt_file) {
    fnt_file = fopen(path_to_fnt_file, "r");
}

void destory_font_file() {
    fclose(fnt_file);
}

static int get_num_characters() {
    int num_characters;
    fseek(fnt_file, 0, SEEK_SET);
    int line_num = 0;
    char buffer[256];
    while(!feof(fnt_file) && line_num < 4) {
        line_num++;
        fgets(buffer, 256, fnt_file);
    }

    const char * const delim = "=";
    char *token;
    token = strtok(buffer, delim);
    token = strtok(NULL, delim);
    file_offset = ftell(fnt_file);
    return atoi(token);
}

static struct Character* get_character() {
    char buffer[256];
    struct Character ch;
    fgets(buffer, 256, fnt_file);
    file_offset = ftell(fnt_file);
    char *token;
    token = strtok(buffer, " ");
    strtok(NULL, " ");
}

int main(int argc, char **argv) {
    if(argc != 2) return 1;
    FILE *file = fopen(argv[1], "r");
    int num = get_num_characters(file);
    printf("%d\n", num);

    CharacterSet set[num-2];

    fclose(file);
    return 0;
}