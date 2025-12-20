#include <glad/glad.h>

#include "chunk.h"
#include "utils.h"

Chunk::Chunk(glm::vec3 position, glm::ivec3 size)
{
    m_size = size;
    m_position = position;
    m_num_indices = 0;
    m_id = generate_uuidv4();
    m_blocks.reserve(m_size.x * m_size.y * m_size.z);

    generate_terrain();
    setup_buffers();
    construct_mesh();
}

Chunk::~Chunk()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void Chunk::setup_buffers()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(m_vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (const void*)offsetof(Vertex, texture_coord));
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT,
        sizeof(Vertex), (const void*)offsetof(Vertex, id));
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
    int index = x + y * m_size.x + z *(m_size.x * m_size.y);

    for (int i = 0; i < 6; i++) {
        glm::ivec3 d = directions[i];
        glm::ivec3 pos(d.x + x, d.y + y, d.z + z);

        // face culling: only render visible block faces
        if (!block_present(pos)) {
            int base_index = vertices.size();
            int vertices_per_face = 4;

            for (int j = 0; j < vertices_per_face; j++) {
                Vertex v = block_vertices[i * vertices_per_face + j];
                v.position += m_position + glm::vec3(x, y, z);
                v.id = m_id + glm::uvec4(0, 0, 0, index);
                // Only set the grass side texture for blocks at the surface of the chunk
                if (y != m_size.y - 1) v.texture_coord.z = 1; // dirt texture index
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

    for (int x = 0; x < m_size.x; x++) {
        for (int y = 0; y < m_size.y; y++) {
            for (int z = 0; z < m_size.z; z++) {
                if (block_present(glm::vec3(x, y, z)))
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

void Chunk::draw()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
}

void Chunk::place_block(
    BlockType block, glm::ivec3 position, glm::ivec3 direction)
{
    glm::vec3 p = block != BlockType::empty ? position + direction : position;
    int index = p.z * (m_size.y * m_size.x) + p.y * m_size.x + p.x;
    bool in_bounds =
        p.x >= 0 && p.x < m_size.x &&
        p.y >= 0 && p.y < m_size.y &&
        p.z >= 0 && p.z < m_size.z;
    if (in_bounds) {
        m_blocks[index] = block;
        construct_mesh();
    }
}

void Chunk::generate_terrain()
{
    m_blocks.clear();
    for (int x = 0; x < m_size.x; x++) {
        for (int y = 0; y < m_size.y; y++) {
            for (int z = 0; z < m_size.z; z++) {
                m_blocks.push_back(BlockType::dirt);
            }
        }
    }
}

bool Chunk::block_present(glm::ivec3 position)
{
    bool in_bounds =
        position.x >= 0 && position.x < m_size.x &&
        position.y >= 0 && position.y < m_size.y &&
        position.z >= 0 && position.z < m_size.z;
    int index =
        position.z * (m_size.y * m_size.x) +
        position.y * m_size.x + position.x;
    return in_bounds && m_blocks[index] != BlockType::empty;
}


// Use the DDA algorithm to step along the ray until a voxel in the chunk is hit
RaycastHit Chunk::raycast(
    glm::vec3 origin, glm::vec3 direction, float max_steps)
{
    RaycastHit result = {
        .position = glm::floor(origin), // start on the current voxel
        .face_normal = glm::vec3(0, 0, 0),
        .did_hit = true
    };

    float steps_taken = 0;
    glm::ivec3 step_dir = glm::sign(direction);    // direction to step in each axis (1 or -1)
    glm::vec3 steps = glm::abs(1.0f / direction); // amount of ray steps to cross one voxel boundary

    // amount of ray steps to reach the next voxel boundary
    glm::vec3 offset = glm::vec3(result.position) + glm::vec3(glm::max(step_dir, glm::ivec3(0)));
    glm::vec3 t_max = (offset - origin) / direction;

    while (steps_taken < max_steps) {
        if (block_present(result.position))
            return result;

        // which grid line do we hit next?
        if (t_max.x < t_max.y) {
            if (t_max.x < t_max.z) {
                // x boundary is closest
                result.position.x += step_dir.x;
                steps_taken = t_max.x;
                t_max.x += steps.x;
                result.face_normal = glm::vec3(-step_dir.x, 0, 0);
            } else {
                // z boundary is closest
                result.position.z += step_dir.z;
                steps_taken = t_max.z;
                t_max.z += steps.z;
                result.face_normal = glm::vec3(0, 0, -step_dir.z);
            }
        } else {
            if (t_max.y < t_max.z) {
                // y boundary is closest
                result.position.y += step_dir.y;
                steps_taken = t_max.y;
                t_max.y += steps.y;
                result.face_normal = glm::vec3(0, -step_dir.y, 0);
            } else {
                // z boundary is closest
                result.position.z += step_dir.z;
                steps_taken = t_max.z;
                t_max.z += steps.z;
                result.face_normal = glm::vec3(0, 0, -step_dir.z);
            }
        }
    }

    result.did_hit = false;
    return result;
}
