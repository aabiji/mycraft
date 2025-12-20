#include "chunk.h"
#include "engine.h"
#include "texture.h"

#include <iostream>

Engine::Engine()
{
    m_platform.open_window("Mycraft", 900, 700, true);
    m_platform.set_callbacks(true);

    m_main_shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    m_id_buffer_shader.load("assets/shaders/id_vertex.glsl", "assets/shaders/id_fragment.glsl");

    m_id_buffer.create(900, 700, 0); // TODO: handle resize
    m_mouse_enabled = true;
}

void Engine::run()
{
    Chunk chunk(glm::vec3(0, 0, 0), glm::ivec3(10, 10, 10));

    TextureAtlas texture;
    texture.create("assets/images/atlas.png", 64, 3, 1);
    int atlas_location = m_main_shader.uniform_location("texture_atlas");
    int id_location = m_main_shader.uniform_location("id_buffer");

    while (!m_platform.window_close()) {
        m_platform.poll_input();

        if (m_platform.key_pressed(GLFW_KEY_W))       m_camera.move(Direction::front);
        if (m_platform.key_pressed(GLFW_KEY_S))       m_camera.move(Direction::back);
        if (m_platform.key_pressed(GLFW_KEY_A))       m_camera.move(Direction::left);
        if (m_platform.key_pressed(GLFW_KEY_D))       m_camera.move(Direction::right);
        if (m_platform.key_pressed(GLFW_KEY_UP))      m_camera.move(Direction::up);
        if (m_platform.key_pressed(GLFW_KEY_DOWN))    m_camera.move(Direction::down);
        if (m_platform.key_released(GLFW_KEY_SPACE))  m_platform.toggle_wireframe_mode();
        if (m_platform.key_released(GLFW_KEY_CAPS_LOCK)) {
            m_mouse_enabled = false;
            m_platform.disable_mouse_input();
        }

        if (m_mouse_enabled) {
            auto [dx, dy] = m_platform.mouse_delta();
            m_camera.rotate(dx, dy);
        }

        auto [x, y] = m_platform.center_position();
        glm::uvec4 selected_id = m_id_buffer.sample(x, y);

        auto [position, direction] = m_camera.ray();
        auto result = chunk.raycast(position, direction, 10);

        if (result.did_hit) {
            if (m_platform.mouse_released(GLFW_MOUSE_BUTTON_LEFT))
                chunk.place_block(BlockType::dirt, result.position, result.face_normal);
            if (m_platform.mouse_released(GLFW_MOUSE_BUTTON_RIGHT))
                chunk.place_block(BlockType::empty, result.position, result.face_normal);
        }

        glm::mat4 model = glm::mat4(1.0);
        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), m_platform.aspect_ratio(), 0.1f, 100.0f);
        glm::mat4 view = m_camera.view_matrix();

        m_id_buffer.enable();
        m_id_buffer_shader.use();
        m_id_buffer_shader.set<glm::mat4>("model", model);
        m_id_buffer_shader.set<glm::mat4>("view", view);
        m_id_buffer_shader.set<glm::mat4>("projection", projection);
        chunk.draw();
        m_id_buffer.disable();

        m_main_shader.use();
        m_main_shader.set<glm::mat4>("model", model);
        m_main_shader.set<glm::mat4>("view", view);
        m_main_shader.set<glm::mat4>("projection", projection);
        m_main_shader.set<glm::uvec4>("current_object_id", selected_id);
        m_platform.clear_frame();
        m_id_buffer.use_texture(id_location);
        texture.use(atlas_location);
        chunk.draw();

        m_platform.present_frame();
    }
}
