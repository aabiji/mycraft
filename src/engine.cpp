#include "chunk.h"
#include "engine.h"
#include "texture.h"

Engine::Engine()
{
    m_platform.open_window("Mycraft", 900, 700, true);
    m_platform.set_callbacks(true);
    m_shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    m_mouse_enabled = true;
}

void Engine::run()
{
    Chunk chunk(glm::vec3(0, 0, 0), glm::ivec3(10, 10, 10));

    TextureAtlas texture;
    texture.create("assets/images/atlas.png", 64, 3, 1);
    int atlas_location = m_shader.uniform_location("texture_atlas");

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

        auto [position, direction] = m_camera.ray();
        auto result = chunk.raycast(position, direction, 25);

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

        m_shader.use();
        m_shader.set<glm::mat4>("model", model);
        m_shader.set<glm::mat4>("view", view);
        m_shader.set<glm::mat4>("projection", projection);
        if (result.did_hit)
            m_shader.set<glm::ivec3>("selected_position", result.position);
        m_platform.clear_frame();
        texture.use(atlas_location);
        chunk.render();

        m_platform.present_frame();
    }
}
