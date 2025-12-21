#pragma once

#include <vector>
#include <utility>

#include "block.h"

enum class BlockType { dirt, empty };

const glm::ivec3 CHUNK_SIZE = glm::ivec3(8, 8, 8);
const int TOTAL_BLOCKS = 8 * 8 * 8;

class Chunk
{
public:
    void release();
    void create(glm::vec3 position);

    void render();
    void place_block(BlockType block, glm::ivec3 position, glm::ivec3 direction);
    bool block_present(glm::ivec3 position);
private:
    void setup_buffers();
    void add_block_vertices(
        std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        int x, int y, int z);
    void construct_mesh();
    std::pair<int, bool> block_index(glm::ivec3 position);

    int m_num_indices;
    unsigned int m_vao, m_vbo, m_ebo;
    BlockType m_blocks[TOTAL_BLOCKS] = { BlockType::empty };
    glm::vec3 m_position;
};
