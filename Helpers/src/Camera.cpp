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
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::milli>(currentTime - lastTime).count();
    lastTime = currentTime;

    if (moveForward) {
        position += speed * deltaTime * direction;
    }
    if (moveBackward) {
        position -= speed * deltaTime * direction;
    }
    if (moveLeft) {
        position -= speed * deltaTime * glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    if (moveRight) {
        position += speed * deltaTime * glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    if (moveUp) {
        position += speed * deltaTime * glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (moveDown) {
        position -= speed * deltaTime * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    if (looking) {
        double pitch = sensitivity * (currentY - lastY);
        double yaw = sensitivity * (currentX - lastX);
        lastX = currentX;
        lastY = currentY;

        direction = glm::normalize(glm::rotate(glm::mat4(1.0f), -(float)yaw, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(direction, 0.0f));
        glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        direction = glm::normalize(glm::rotate(glm::mat4(1.0f), -(float)pitch, right) * glm::vec4(direction, 0.0f));
    }
}
