#include "Camera.h"

// constructor with vectors
Camera::Camera(glm::vec3 position0, glm::vec3 up0, float yaw0, float pitch0) :
	state(WALKING),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED),
	runningSpeed(RUNSPEED),
	mouseSensitivity(SENSITIVITY),
	fov(FOV)
{
    position = position0;
    worldUp = up0;
    yaw = yaw0;
    pitch = pitch0;
    updateCameraVectors();
}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
	state(WALKING),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED),
	runningSpeed(RUNSPEED),
	mouseSensitivity(SENSITIVITY),
	fov(FOV)
{
    position = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const{
    return glm::lookAt(position, position + front, up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(const Camera_Movement direction){
    float velocity;
    if (state == WALKING) {
        velocity = movementSpeed * static_cast<float>(Time::deltaTime);
    } else {
        velocity = runningSpeed * static_cast<float>(Time::deltaTime);
    }

    switch (direction)
    {
    case FORWARD:
        position += front * velocity;
        break;
    case BACKWARD:
        position -= front * velocity;
        break;
    case LEFT:
        position -= right * velocity;
        break;
    case RIGHT:
        position += right * velocity;
        break;
    case UPWARD:
        //position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        position += up * velocity;
        break;
    case DOWNWARD:
        //position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        position -= up * velocity;
        break;
    case RUN:
        state = RUNNING;
        break;
    case WALK:
        state = WALKING;
        break;
    }       
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, const GLboolean constrainPitch){
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;
    
    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch){
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(const float yoffset){
    fov -= static_cast<float>(yoffset);
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > FOV)
        fov = FOV;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors(){
    // calculate the new Front vector
    glm::vec3 front0;
    front0.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front0.y = sin(glm::radians(pitch));
    front0.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front0);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}