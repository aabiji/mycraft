#version 460 core

in vec3 texture_coord;

out vec4 fragment_color;

uniform sampler2DArray texture_atlas;

float neighbor(float x, float y)
{
    // convert to grayscale
    vec3 luminosity = vec3(0.299, 0.587, 0.114);
    vec3 pixel = texture(texture_atlas, vec3(x, y, texture_coord.z)).xyz;
    return dot(pixel, luminosity);
}

void main()
{
    // Apply a Laplace kernel to the pixel's neighborhood to determine
    // whether the current pixel differs sharply from its neighbors
    float l = neighbor(texture_coord.x - 0.01, texture_coord.y);
    float r = neighbor(texture_coord.x + 0.01, texture_coord.y);
    float t = neighbor(texture_coord.x, texture_coord.y - 0.01);
    float b = neighbor(texture_coord.x, texture_coord.y + 0.01);
    float c = neighbor(texture_coord.x, texture_coord.y);
    float value = abs(t + l + c * -4 + r + b);
    bool is_edge = value >= 0.01;

    fragment_color = is_edge
        ? vec4(1.0, 1.0, 1.0, 1.0)
        : texture(texture_atlas, texture_coord);
}
