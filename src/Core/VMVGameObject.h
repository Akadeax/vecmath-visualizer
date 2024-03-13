#ifndef VMV_VMVGAMEOBJECT_H
#define VMV_VMVGAMEOBJECT_H

#include "VMVModel.h"
#include <cstdint>
#include <memory>

namespace vmv
{
    struct Transform
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 GetMat() const;
        glm::mat3 NormalMatrix() const;
    };

    class VMVGameObject final
    {
      public:
        using id_t = uint32_t;
        id_t GetId() const { return m_Id; }

        static VMVGameObject CreateGameObject();

        VMVGameObject(const VMVGameObject&) = delete;
        VMVGameObject(VMVGameObject&&) noexcept = default; // default
        VMVGameObject& operator=(const VMVGameObject&) = delete;
        VMVGameObject& operator=(VMVGameObject&&) noexcept = default; // default

        std::shared_ptr<VMVModel> m_Model;
        glm::vec3 m_Color{};

        Transform m_Transform{};

      private:
        VMVGameObject(id_t id);

        id_t m_Id;
    };
} // namespace vmv

#endif