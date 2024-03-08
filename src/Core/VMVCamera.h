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

        const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

      private:
        glm::mat4 m_ProjectionMatrix{1.f};
    };
} // namespace vmv

#endif