#include "terrain.h"

Terrain::~Terrain()
{
    for (auto& [key, value] : m_chunks)
        value.release();
}

Location Terrain::get_location(glm::vec3 global_position)
{
    glm::ivec3 chunk_position = glm::ivec3(global_position) / CHUNK_SIZE;
    glm::ivec3 position = glm::ivec3(global_position) - (chunk_position * CHUNK_SIZE);
    bool in_range = m_chunks.count(chunk_position);
    return { chunk_position, position, in_range };
}

void Terrain::generate(glm::ivec3 current_position)
{
    int radius = 1;
    auto [ chunk_pos, _, __ ] = get_location(current_position);
    int start_x = chunk_pos.x - radius, end_x = chunk_pos.x + radius;
    int start_y = chunk_pos.y - radius, end_y = chunk_pos.y + radius;
    int start_z = chunk_pos.z - radius, end_z = chunk_pos.z + radius;

    for (int x = start_x; x <= end_x; x++) {
        //for (int y = start_y; y <= end_y; y++) {
            for (int z = start_z; z <= end_z; z++) {
                glm::ivec3 pos = glm::ivec3(x, 0, z);
                if (!m_chunks.count(pos)) {
                    Chunk chunk = Chunk();
                    chunk.create(pos * CHUNK_SIZE);
                    m_chunks.insert({pos, std::move(chunk)});
                }
            }
        //}
    }
}

void Terrain::render(glm::ivec3 current_position)
{
    int radius = 1;
    auto [ chunk_pos, _, __ ] = get_location(current_position);
    int start_x = chunk_pos.x - radius, end_x = chunk_pos.x + radius;
    int start_y = chunk_pos.y - radius, end_y = chunk_pos.y + radius;
    int start_z = chunk_pos.z - radius, end_z = chunk_pos.z + radius;

    for (int x = start_x; x <= end_x; x++) {
        //for (int y = start_y; y <= end_y; y++) {
            for (int z = start_z; z <= end_z; z++) {
                glm::ivec3 pos = glm::ivec3(x, 0, z);
                if (m_chunks.count(pos))
                    m_chunks.at(pos).render();

            }
        //}
    }
}

void Terrain::place_block(BlockType block, glm::ivec3 position, glm::ivec3 direction)
{
    auto [chunk_pos, in_chunk_pos, in_range] = get_location(position);
    if (in_range)
        m_chunks.at(chunk_pos).place_block(block, in_chunk_pos, direction);
}

// Use the DDA algorithm to step along the ray until a voxel in the chunk is hit
RaycastHit Terrain::raycast(
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
        auto [chunk_pos, position, in_range] = get_location(result.position);
        bool block_present =
            in_range ? m_chunks.at(chunk_pos).block_present(position) : false;
        if (block_present) return result;

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
