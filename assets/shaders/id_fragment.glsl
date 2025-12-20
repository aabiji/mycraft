#version 460 core

flat in uvec4 object_id;
layout (location = 0) out uvec4 output_value;

void main()
{
    output_value = object_id;
}
