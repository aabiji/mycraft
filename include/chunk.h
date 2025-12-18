#pragma once

#include <vector>

#include "block.h"

enum class BlockType { dirt, empty };

class Chunk
{
public:
    Chunk(int width, int height, int depth);
    ~Chunk();

    void draw();
private:
    void setup_buffers();
    void add_block_vertices(
        std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        int x, int y, int z);
    void construct_mesh();

    void generate_terrain();
    bool block_present(int x, int y, int z);

    int m_num_indices;
    glm::ivec3 m_size;
    unsigned int m_vao, m_vbo, m_ebo;
    std::vector<BlockType> m_blocks;
};
