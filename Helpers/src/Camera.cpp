//
// Created by User on 3/14/2025.
//

#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction):
    position(position),
    direction(direction)
{

}

void Camera::move() {
    if (moveForward) {
        position += speed * direction;
    }
    if (moveBackward) {
        position -= speed * direction;
    }
    if (moveLeft) {
        position -= speed * glm::normalize(glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f)));
    }
    if (moveRight) {
        position += speed * glm::normalize(glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f)));
    }
    if (moveUp) {
        position += speed * glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (moveDown) {
        position -= speed * glm::vec3(0.0f, 0.0f, 1.0f);
    }

    if (looking) {
        double pitch = sensitivity * (currentY - lastY);
        double yaw = sensitivity * (currentX - lastX);
        lastX = currentX;
        lastY = currentY;

        direction = glm::normalize(glm::rotate(glm::mat4(1.0f), -(float)yaw, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4(direction, 0.0f));
        glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f)));
        direction = glm::normalize(glm::rotate(glm::mat4(1.0f), -(float)pitch, right) * glm::vec4(direction, 0.0f));
    }
}
