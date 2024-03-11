#ifndef VMV_VMVCAMERA_H
#define VMV_VMVCAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vmv
{
    class VMVCamera final
    {
      public:
        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void SetPerspectiveProjection(float fovY, float aspect, float near, float far);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewEuler(glm::vec3 position, glm::vec3 rotationEuler);
        const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
        const glm::mat4& GetView() const { return m_ViewMatrix; }

      private:
        glm::mat4 m_ProjectionMatrix{1.f};
        glm::mat4 m_ViewMatrix{1.f};
    };
} // namespace vmv

#endif