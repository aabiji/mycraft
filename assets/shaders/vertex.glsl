#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_texture_coord;
layout (location = 3) in ivec3 in_voxel_position;

out vec3 texture_coord;
flat out ivec3 voxel_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
    texture_coord = in_texture_coord;
    voxel_position = in_voxel_position;
}
