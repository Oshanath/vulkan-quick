//
// Created by User on 3/14/2025.
//

#ifndef CAMERA_H
#define CAMERA_H
#include "glm/ext/matrix_transform.hpp"

#include <iostream>
#include <chrono>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    float speed = 7.0f;
    float sensitivity = 0.001f;

    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool moveUp = false;
    bool moveDown = false;

    double lastX = 0.0f;
    double lastY = 0.0f;
    double currentX = 0.0f;
    double currentY = 0.0f;

    bool looking = false;
    std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::high_resolution_clock::now();

    Camera(){}
    Camera(glm::vec3 position, glm::vec3 direction);
    void move();
    void print() {
        std::cout << "Position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
        std::cout << "Direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
    }

    [[nodiscard]] glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};



#endif //CAMERA_H
