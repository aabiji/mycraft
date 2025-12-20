#pragma once

class TextureAtlas
{
public:
    ~TextureAtlas();
    void use(int tetxure_location);
    void create(const char* path,
        int sprite_size, int num_sprites, int texture_index);
private:
    int m_texture_index;
    unsigned int m_id;
};
