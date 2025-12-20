#version 460 core

in vec3 texture_coord;
flat in ivec3 voxel_position;

out vec4 fragment_color;

uniform ivec3 selected_position;
uniform usampler2D id_buffer;
uniform sampler2DArray texture_atlas;

void main()
{
    vec3 color = texture(texture_atlas, texture_coord).xyz;
    vec3 lighter = color + vec3(1.0, 1.0, 1.0) * 0.05;
    fragment_color = voxel_position == selected_position
        ? vec4(lighter, 1.0) : vec4(color, 1.0);
}
