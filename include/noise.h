#pragma once

#ifndef NOISE_IMPLEMENTATION
float fractal_noise(float x, float y, float tilesize);
#else
#include <cmath>
#include <glm/glm.hpp>

inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
inline float lerp(float a, float b, float t) { return (1 - t) * a + t * b; }

// Using xxHash for deterministic randomness
int hash(int x, int y)
{
    int h = x * 3266489917 + y * 668265263;
    h ^= h >> 15;
    h *= 2246822519;
    h ^= h >> 13;
    h *= 3266489917;
    h ^= h >> 16;
    return h;
}

// Create a random gradient vector
glm::vec2 gradient(int x, int y)
{
    int h = hash(x, y) & 255;
    float angle = (h / 255.0f) * 2.0f * M_PI;
    return glm::vec2{std::cos(angle), std::sin(angle)};
}

float perlin(float x, float y)
{
    // get the grid cell the point's in and
    // the direction of the point in that grid cell
    int X = std::floor(x);
    int Y = std::floor(y);
    float dx = x - X;
    float dy = y - Y;

    // get the gradient vectors for each corner
    glm::vec2 gtl = gradient(X, Y);
    glm::vec2 gtr = gradient(X + 1, Y);
    glm::vec2 gbl = gradient(X, Y + 1);
    glm::vec2 gbr = gradient(X + 1, Y + 1);

    // compute dot products to get the gradient values for each corner
    float vtl = gtl.x * dx       + gtl.y * dy;
    float vtr = gtr.x * (dx - 1) + gtr.y * dy;
    float vbl = gbl.x * dx       + gbl.y * (dy - 1);
    float vbr = gbr.x * (dx - 1) + gbr.y * (dy - 1);

    // interpolate those values
    float a = lerp(vtl, vtr, fade(dx));
    float b = lerp(vbl, vbr, fade(dx));
    float noise = lerp(a, b, fade(dy));
    return noise * 0.7f + 0.5f;
}

float fractal_noise(float x, float y, float tilesize)
{
    float total = 0.0;
    float max_value = 0.0;
    float frequency = 1.0;
    float amplitude = 1.0;
    float persistence = 0.5;
    float lacunarity = 2.0;

    // layer perlin noise together
    for (int octave = 0; octave < 6; octave++) {
        float sample_x = fmod(x * frequency, tilesize);
        float sample_y = fmod(y * frequency, tilesize);

        total += perlin(sample_x, sample_y) * amplitude;
        max_value += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return total / max_value;
}
#endif