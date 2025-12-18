#version 460 core

in vec3 texture_coord;

out vec4 fragment_color;

//uniform sampler3D block_spritesheet;

void main()
{
    fragment_color = vec4(1.0, 1.0, 0.0, 1.0);
    //fragment_color = texture(block_spritesheet, texture_coord);
}
