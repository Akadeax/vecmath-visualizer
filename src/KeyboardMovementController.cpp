#include "KeyboardMovementController.h"

#include <glm/gtc/constants.hpp>

void vmv::KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, float dt, VMVGameObject& go)
{
    glm::vec3 rotate(0.f);
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
        rotate.y += 1.f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
        rotate.y -= 1.f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
        rotate.x += 1.f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
        rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        go.m_Transform.rotation += lookSpeed * dt * glm::normalize(rotate);

    go.m_Transform.rotation.x = glm::clamp(go.m_Transform.rotation.x, -1.5f, 1.5f);
    go.m_Transform.rotation.y = glm::mod(go.m_Transform.rotation.y, glm::two_pi<float>());

    float yaw{go.m_Transform.rotation.y};
    const glm::vec3 forward{std::sinf(yaw), 0.f, std::cosf(yaw)};
    const glm::vec3 right{forward.z, 0.f, -forward.x};
    const glm::vec3 up{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
        moveDir += forward;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
        moveDir -= forward;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
        moveDir += right;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
        moveDir -= right;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
        moveDir += up;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
        moveDir -= up;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        go.m_Transform.translation += moveSpeed * dt * glm::normalize(moveDir);
}