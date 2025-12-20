#version 460 core

in vec3 texture_coord;
out vec4 fragment_color;
uniform sampler2DArray texture_atlas;

void main()
{
    fragment_color = texture(texture_atlas, texture_coord);
}
