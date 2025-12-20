#version 460 core

flat in uint object_id;
out uint output_value;

void main()
{
    output_value = object_id;
}
