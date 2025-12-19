#pragma once

class TextureAtlas
{
public:
    ~TextureAtlas();
    void use(int sampler_location);
    void create(const char* path,
        int sprite_size, int num_sprites, int sampler);
private:
    int m_sampler;
    unsigned int m_id;
};
