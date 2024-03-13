#ifndef VMV_VECMATHVISUALIZER_H
#define VMV_VECMATHVISUALIZER_H

#include "Core/VMVCamera.h"
#include "Core/VMVDevice.h"
#include "Core/VMVGameObject.h"
#include "Core/VMVModel.h"
#include "Core/VMVRenderer.h"
#include "Core/VMVWindow.h"
#include <memory>
#include <vector>

namespace vmv
{
    class VecmathVisualizer
    {
      public:
        VecmathVisualizer();
        ~VecmathVisualizer();

        VecmathVisualizer(const VecmathVisualizer&) = delete;
        VecmathVisualizer(VecmathVisualizer&&) noexcept = delete;
        VecmathVisualizer& operator=(const VecmathVisualizer&) = delete;
        VecmathVisualizer& operator=(VecmathVisualizer&&) noexcept = delete;

        static constexpr uint32_t WIDTH{800};
        static constexpr uint32_t HEIGHT{600};

        void Run();

      private:
        VMVWindow m_VMVWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        VMVDevice m_VMVDevice{m_VMVWindow};
        VMVRenderer m_VMVRenderer{m_VMVWindow, m_VMVDevice};

        std::vector<VMVGameObject> m_GameObjects;

        void LoadGameObjects();
    };
} // namespace vmv

#endif