#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTION
#include "../include/stb_image.h"

int main() {
    // int w, h, n;
    stbi_set_flip_vertically_on_load(true);
    // unsigned char *data = stbi_load("res/dice.png", &w, &h, &n, 0);
    // stbi_image_free(data);
    return 0;
}