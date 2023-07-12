#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Time.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UPWARD,
    DOWNWARD,
    RUN,
    WALK
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 15.0f;
const float RUNSPEED = 30.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 60.0f;

class Camera
{
private:
    enum State {
        WALKING,
        RUNNING
    };
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
    State state;
public:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float runningSpeed;
    float mouseSensitivity;
    float fov;//to zoom

    // constructor with vectors
    Camera(glm::vec3 position0 = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up0 = glm::vec3(0.0f, 1.0f, 0.0f), float yaw0 = YAW, float pitch0 = PITCH);

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;

    void ProcessKeyboard(Camera_Movement direction);

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);
};