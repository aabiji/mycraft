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

    m_id_buffer.create(900, 700); // TODO: handle resize
    m_mouse_enabled = true;
}

void Engine::run()
{
    Chunk chunk(glm::vec3(0, 0, 0), glm::ivec3(10, 10, 10));

    TextureAtlas texture;
    texture.create("assets/images/atlas.png", 64, 3, 0);
    int texture_location = m_main_shader.uniform_location("texture_atlas");

    while (!m_platform.window_close()) {
        m_platform.begin_frame();

        if (m_platform.key_pressed(GLFW_KEY_W))       m_camera.move(Direction::front);
        if (m_platform.key_pressed(GLFW_KEY_S))       m_camera.move(Direction::back);
        if (m_platform.key_pressed(GLFW_KEY_A))       m_camera.move(Direction::left);
        if (m_platform.key_pressed(GLFW_KEY_D))       m_camera.move(Direction::right);
        if (m_platform.key_pressed(GLFW_KEY_UP))      m_camera.move(Direction::up);
        if (m_platform.key_pressed(GLFW_KEY_DOWN))    m_camera.move(Direction::down);
        if (m_platform.key_released(GLFW_KEY_SPACE))  m_platform.toggle_wireframe_mode();
        if (m_platform.key_released(GLFW_KEY_CAPS_LOCK)) {
            m_mouse_enabled = false;
            m_platform.release_mouse();
        }

        if (m_mouse_enabled) {
            auto [dx, dy] = m_platform.mouse_delta();
            m_camera.rotate(dx, dy);
        }

        auto [x, y] = m_platform.center_position();
        std::cout << "Current object id: " << m_id_buffer.sample(x, y) << "\n";

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
        m_platform.clear_frame();
        texture.use(texture_location);
        chunk.draw();

        m_platform.present_frame();
    }
}
