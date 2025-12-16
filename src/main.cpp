#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>

#include "camera.h"
#include "platform.h"
#include "shader.h"

int main()
{
    try {
        Camera camera;
        ShaderProgram shader;
        InputState state;
        Platform platform;

        platform.open_window("Mycraft", 900, 700, true);
        platform.set_callbacks(true);

        shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

        while (!platform.window_close()) {
            platform.clear_window();

            InputState state = platform.state;
            if (state.key_presses[GLFW_KEY_W])     camera.move(Direction::front);
            if (state.key_presses[GLFW_KEY_S])     camera.move(Direction::back);
            if (state.key_presses[GLFW_KEY_A])     camera.move(Direction::left);
            if (state.key_presses[GLFW_KEY_D])     camera.move(Direction::right);
            if (state.key_presses[GLFW_KEY_UP])    camera.move(Direction::up);
            if (state.key_presses[GLFW_KEY_DOWN])  camera.move(Direction::down);
            if (state.key_presses[GLFW_KEY_SPACE]) platform.toggle_wireframe_mode();

            camera.rotate(state.mouse_delta_x, state.mouse_delta_y);

            // TODO: does toggling wireframe work properly?

            glm::mat4 model = glm::mat4(1.0);
            glm::mat4 projection =
                glm::perspective(glm::radians(45.0f), platform.aspect_ratio(), 0.1f, 100.0f);
            glm::mat4 view = camera.view_matrix();

            shader.use();
            shader.set<glm::mat4>("model", model);
            shader.set<glm::mat4>("view", view);
            shader.set<glm::mat4>("projection", projection);

            platform.present_window();
        }
    } catch (std::runtime_error& error) {
        std::cout << error.what() << "\n";
    }

    return 0;
}
