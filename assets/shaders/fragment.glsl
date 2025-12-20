#version 460 core

in vec3 texture_coord;
out vec4 fragment_color;

uniform usampler2D id_buffer;
uniform sampler2DArray texture_atlas;

uniform uvec4 current_object_id;

void main()
{
    ivec2 pos = ivec2(gl_FragCoord.xy);
    uvec4 t = texelFetch(id_buffer, pos + ivec2(0,-1), 0);
    uvec4 b = texelFetch(id_buffer, pos + ivec2(0, 1), 0);
    uvec4 l = texelFetch(id_buffer, pos + ivec2(-1,0), 0);
    uvec4 r = texelFetch(id_buffer, pos + ivec2(1, 0), 0);
    uvec4 c = texelFetch(id_buffer, pos, 0);
    bool is_edge = (c != t) || (c != b) || (c != l) || (c != r);
    bool is_outline = is_edge && c == current_object_id;

    fragment_color = is_outline
        ? vec4(1.0, 1.0, 1.0, 1.0)
        : texture(texture_atlas, texture_coord);
}
