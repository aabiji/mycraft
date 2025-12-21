#include <random>
#include <glad/glad.h>

#include "chunk.h"
#define NOISE_IMPLEMENTATION
#include "noise.h"

void Chunk::create(glm::vec3 position)
{
    m_position = position;
    m_num_indices = 0;

    /*
    // procedurally generate the chunk
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    for (int x = 0; x < CHUNK_SIZE.x; x++) {
        for (int z = 0; z < CHUNK_SIZE.z; z++) {
            float sample_x = m_position.x + x + dist(gen);
            float sample_z = m_position.z + z + dist(gen);
            int height = std::ceil(fractal_noise(sample_x, sample_z, CHUNK_SIZE.x) * CHUNK_SIZE.y);
            for (int y = 0; y < height; y++) {
                auto [index, _] = block_index(glm::ivec3(x, y, z));
                m_blocks[index] = BlockType::dirt;
            }
        }
    }
    */

    for (int x = 0; x < CHUNK_SIZE.x; x++) {
        for (int y = 0; y < CHUNK_SIZE.y; y++) {
            for (int z = 0; z < CHUNK_SIZE.z; z++) {
                auto [index, _] = block_index(glm::ivec3(x, y, z));
                m_blocks[index] = BlockType::dirt;
            }
        }
    }

    setup_buffers();
    construct_mesh();
}

void Chunk::release()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Chunk::render()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
}

std::pair<int, bool> Chunk::block_index(glm::ivec3 p)
{
    //int index = (p.x * CHUNK_SIZE.x * CHUNK_SIZE.y) + (p.y * CHUNK_SIZE.y) + p.z;
    int index = p.z * (CHUNK_SIZE.x * CHUNK_SIZE.y) + p.y * CHUNK_SIZE.x + p.x;
    return { index, index >= 0 && index < TOTAL_BLOCKS };
}

bool Chunk::block_present(glm::ivec3 position)
{
    auto [index, in_range] = block_index(position);
    return in_range && m_blocks[index] != BlockType::empty;
}

void Chunk::place_block(
    BlockType block, glm::ivec3 position, glm::ivec3 direction)
{
    auto [index, in_range] =
        block_index(block != BlockType::empty ? position + direction : position);
    if (in_range) {
        m_blocks[index] = block;
        construct_mesh();
    }
}

void Chunk::setup_buffers()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (const void*)offsetof(Vertex, texture_coord));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 3, GL_UNSIGNED_INT,
        sizeof(Vertex), (const void*)offsetof(Vertex, voxel_position));
    glEnableVertexAttribArray(3);
}

void Chunk::add_block_vertices(
    std::vector<Vertex>& vertices,
    std::vector<unsigned int>& indices,
    int x, int y, int z)
{
    // NOTE: must match order in block_vertices (block.h)
    glm::ivec3 directions[] = {
        glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), // left/right
        glm::vec3( 0, 1, 0), glm::vec3(0,-1, 0), // top/bottom
        glm::vec3( 0, 0, 1), glm::vec3(0, 0,-1), // front/back
    };

    for (int i = 0; i < 6; i++) {
        glm::vec3 voxel_position = m_position + glm::vec3(x, y, z);
        glm::ivec3 d = directions[i];
        glm::ivec3 pos(d.x + x, d.y + y, d.z + z);

        // face culling: only render visible block faces
        if (!block_present(pos)) {
            int base_index = vertices.size();
            int vertices_per_face = 4;

            for (int j = 0; j < vertices_per_face; j++) {
                Vertex v = block_vertices[i * vertices_per_face + j];
                v.position += voxel_position;
                v.voxel_position = voxel_position;

                // Only set the grass side texture for blocks at the surface of the chunk
                if (y != CHUNK_SIZE.y - 1) v.texture_coord.z = 1; // dirt texture index

                vertices.push_back(v);
            }

            for (int j = 0; j < 6; j++)
                indices.push_back(base_index + face_indices[j]);
        }
    }
}

void Chunk::construct_mesh()
{
    // create the mesh
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int x = 0; x < CHUNK_SIZE.x; x++) {
        for (int y = 0; y < CHUNK_SIZE.y; y++) {
            for (int z = 0; z < CHUNK_SIZE.z; z++) {
                if (block_present(glm::ivec3(x, y, z)))
                    add_block_vertices(vertices, indices, x, y, z);
            }
        }
    }

    // update the buffers
    int vertices_size = vertices.size() * sizeof(Vertex);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size, vertices.data());

    m_num_indices = indices.size();
    int indices_size = indices.size() * sizeof(unsigned int);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_size, indices.data());
}

