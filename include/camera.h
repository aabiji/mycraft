#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Direction { front, back, left, right, up, down };

class Camera
{
public:
    Camera() :
        m_position(0.0, 0.0, 3.0), m_front(0.0, 0.0, 0.0),
        m_up(0.0, 1.0, 0.0), m_prev_x(-1), m_prev_y(-1),
        m_pitch(0), m_yaw(0) {}

    void rotate(float mousex, float mousey)
    {
        if (m_prev_x == -1 && m_prev_y == -1) {
            m_prev_x = mousex;
            m_prev_y = mousey;
            return;
        }

        float sensitivity = 0.1;
        float x_offset = (mousex - m_prev_x) * sensitivity;
        float y_offset = (m_prev_y - mousey) * sensitivity;

        m_pitch = fmin(fmax(-89, m_pitch + y_offset), 89);
        m_yaw += x_offset;
        m_prev_x = mousex;
        m_prev_y = mousey;

        glm::vec3 direction;
        direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y = sin(glm::radians(m_pitch));
        direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(direction);
    }

    void move(Direction direction)
    {
        float speed = 0.05;
        glm::vec3 side = glm::normalize(glm::cross(m_front, m_up));

        if (direction == Direction::front) m_position += speed * m_front;
        if (direction == Direction::back)  m_position -= speed * m_front;
        if (direction == Direction::right) m_position += speed * side;
        if (direction == Direction::left)  m_position -= speed * side;
        if (direction == Direction::up)    m_position -= speed * m_up;
        if (direction == Direction::down)  m_position -= speed * m_up;
    }

    glm::mat4 view_matrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
private:
    float m_pitch, m_yaw;
    float m_prev_x, m_prev_y;
    glm::vec3 m_position, m_front, m_up;
};
