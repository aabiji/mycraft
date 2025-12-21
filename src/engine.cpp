#include "chunk.h"
#include "engine.h"
#include "texture.h"

Engine::Engine()
{
    m_platform.open_window("Mycraft", 900, 700, true);
    m_shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    m_texture_atlas.create("assets/images/atlas.png", 64, 3, 1);
    m_terrain.generate(glm::ivec3(0, 0, 0));
    m_mouse_enabled = true;
}

void Engine::handle_input()
{
    if (m_platform.input_pressed(GLFW_KEY_W, false))       m_camera.move(Direction::front);
    if (m_platform.input_pressed(GLFW_KEY_S, false))       m_camera.move(Direction::back);
    if (m_platform.input_pressed(GLFW_KEY_A, false))       m_camera.move(Direction::left);
    if (m_platform.input_pressed(GLFW_KEY_D, false))       m_camera.move(Direction::right);
    if (m_platform.input_pressed(GLFW_KEY_UP, false))      m_camera.move(Direction::up);
    if (m_platform.input_pressed(GLFW_KEY_DOWN, false))    m_camera.move(Direction::down);
    if (m_platform.input_released(GLFW_KEY_SPACE, false))  m_platform.toggle_wireframe_mode();
    if (m_platform.input_released(GLFW_KEY_CAPS_LOCK, false)) m_platform.set_mouse_mode(false);

    auto [dx, dy] = m_platform.mouse_delta();
    m_camera.rotate(dx, dy);

    auto [position, direction] = m_camera.ray();
    m_hit = m_terrain.raycast(position, direction, 25);

    if (m_hit.did_hit) {
        if (m_platform.input_released(GLFW_MOUSE_BUTTON_LEFT, true))
            m_terrain.place_block(BlockType::dirt, m_hit.position, m_hit.face_normal);

        if (m_platform.input_released(GLFW_MOUSE_BUTTON_RIGHT, true))
            m_terrain.place_block(BlockType::empty, m_hit.position, m_hit.face_normal);
    }
}

void Engine::set_shader_state()
{
    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f), m_platform.aspect_ratio(), 0.1f, 100.0f);
    glm::mat4 view = m_camera.view_matrix();

    m_shader.use();
    m_shader.set<glm::mat4>("model", model);
    m_shader.set<glm::mat4>("view", view);
    m_shader.set<glm::mat4>("projection", projection);
    if (m_hit.did_hit)
        m_shader.set<glm::ivec3>("selected_position", m_hit.position);
}

void Engine::run()
{
    while (!m_platform.should_close()) {
        m_platform.start_frame();
        handle_input();
        set_shader_state();

        int atlas_location = m_shader.uniform_location("texture_atlas");
        m_texture_atlas.use(atlas_location);
        m_terrain.render(glm::vec3(0, 0, 0));
        m_platform.end_frame();
    }
}
