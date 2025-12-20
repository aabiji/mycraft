#include <algorithm>

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "error.h"
#include "texture.h"

TextureAtlas::~TextureAtlas() { glDeleteTextures(1, &m_id); }

void TextureAtlas::use(int texture_location)
{
    glActiveTexture(GL_TEXTURE0 + m_texture_index);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
    glUniform1i(texture_location, m_texture_index);
}

void TextureAtlas::create(
    const char* path, int sprite_size, int num_sprites, int texture_index)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* pixels =
        stbi_load(path, &width, &height, &channels, 4);
    bool in_range =
        num_sprites <= (width / sprite_size) * (height / sprite_size);

    if (!pixels)
        throw Error("Failed to read {}", path);
    if (!in_range) {
        stbi_image_free(pixels);
        throw Error("Too many sprites");
    }

    // create the texture array
    m_texture_index = texture_index;
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, sprite_size, sprite_size, num_sprites);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // read the individual sprites from the spritesheet
    // and place into the texture array
    int x = 0, y = 0;
    for (int layer = 0; layer < num_sprites; layer++) {
        int size = sprite_size * sprite_size * 4;
        unsigned char* cropped = new unsigned char[size];

        for (int row = 0; row < sprite_size; row++) {
            int start = ((y + row) * width + x) * 4;
            int end = start + sprite_size * 4;
            int destination = row * sprite_size * 4;
            std::copy(pixels + start, pixels + end, cropped + destination);
        }

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0,
            layer, sprite_size, sprite_size, 1,
            GL_RGBA, GL_UNSIGNED_BYTE, cropped);
        delete[] cropped;

        x += sprite_size;
        if (x >= width) {
            x = 0;
            y += sprite_size;
        }
    }

    stbi_image_free(pixels);
}
