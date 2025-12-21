#pragma once

#include <unordered_map>

#include "chunk.h"

struct RaycastHit
{
    glm::ivec3 position;
    glm::ivec3 face_normal;
    bool did_hit;
};

struct Location
{
    glm::ivec3 chunk_position;
    glm::ivec3 position; // in the chunk
    bool in_range;
};

struct ivec3_hash {
    std::size_t operator()(const glm::ivec3& v) const {
        return ((std::hash<int>()(v.x) ^
                (std::hash<int>()(v.y) << 1)) >> 1) ^
                (std::hash<int>()(v.z) << 1);
    }
};

class Terrain
{
public:
    ~Terrain();

    void render(glm::ivec3 current_position);
    void generate(glm::ivec3 current_position);
    RaycastHit raycast(glm::vec3 origin, glm::vec3 direction, float max_steps);
    void place_block(BlockType block, glm::ivec3 position, glm::ivec3 direction);
private:
    Location get_location(glm::vec3 global_position);

    std::unordered_map<glm::ivec3, Chunk, ivec3_hash> m_chunks;
};