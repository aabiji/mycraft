#pragma once

#include <vector>

#include "block.h"

enum class BlockType { dirt, empty };

struct RaycastHit
{
    glm::ivec3 position;
    glm::ivec3 face_normal;
    bool did_hit;
};

class Chunk
{
public:
    Chunk(glm::vec3 position, glm::ivec3 size);
    ~Chunk();

    void draw();
    void place_block(BlockType block, glm::ivec3 position, glm::ivec3 direction);
    RaycastHit raycast(glm::vec3 origin, glm::vec3 direction, float max_steps);
private:
    void setup_buffers();
    void add_block_vertices(
        std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        int x, int y, int z);
    void construct_mesh();

    void generate_terrain();
    bool block_present(glm::ivec3 position);

    glm::uvec4 m_id;
    int m_num_indices;
    glm::ivec3 m_size;
    glm::vec3 m_position;
    unsigned int m_vao, m_vbo, m_ebo;
    std::vector<BlockType> m_blocks;
};
