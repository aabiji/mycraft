#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.h"

class Camera
{
public:
    Camera() :
        m_position(0.0, 0.0, 10.0), m_front(0.0, 0.0, -1.0),
        m_up(0.0, 1.0, 0.0), m_pitch(0), m_yaw(0) {}

    void rotate(float mouse_delta_x, float mouse_delta_y)
    {
        float sensitivity = 0.35;
        float x_offset = mouse_delta_x * sensitivity;
        float y_offset = mouse_delta_y * sensitivity;

        m_pitch = fmin(fmax(-89, m_pitch + y_offset), 89);
        m_yaw += x_offset;

        glm::vec3 direction;
        direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y = sin(glm::radians(m_pitch));
        direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(direction);
    }

    void move(Direction direction)
    {
        float speed = 0.2;
        glm::vec3 side = glm::normalize(glm::cross(m_front, m_up));

        switch (direction) {
            case Direction::front: m_position += speed * m_front; break;
            case Direction::back:  m_position -= speed * m_front; break;
            case Direction::right: m_position += speed * side; break;
            case Direction::left:  m_position -= speed * side; break;
            case Direction::up:    m_position += speed * m_up; break;
            case Direction::down:  m_position -= speed * m_up; break;
        }
    }

    glm::mat4 view_matrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }
private:
    float m_pitch;
    float m_yaw;
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
};
