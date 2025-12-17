#include <iostream>
#include <stdexcept>

#include "camera.h"
#include "cube.h"
#include "platform.h"
#include "shader.h"

int main()
{
    try {
        Platform platform;
        ShaderProgram shader;
        Camera camera;

        platform.open_window("Mycraft", 900, 700, true);
        platform.set_callbacks(true);
        shader.load("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

        while (!platform.window_close()) {
            platform.begin_frame();

            if (platform.key_pressed(GLFW_KEY_W))      camera.move(Direction::front);
            if (platform.key_pressed(GLFW_KEY_S))      camera.move(Direction::back);
            if (platform.key_pressed(GLFW_KEY_A))      camera.move(Direction::left);
            if (platform.key_pressed(GLFW_KEY_D))      camera.move(Direction::right);
            if (platform.key_pressed(GLFW_KEY_UP))     camera.move(Direction::up);
            if (platform.key_pressed(GLFW_KEY_DOWN))   camera.move(Direction::down);
            if (platform.key_released(GLFW_KEY_SPACE)) platform.toggle_wireframe_mode();

            auto [dx, dy] = platform.mouse_delta();
            camera.rotate(dx, dy);

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
    } catch (const std::runtime_error& error) {
        std::cout << error.what() << "\n";
    }

    Platform::shutdown();
    return 0;
}
