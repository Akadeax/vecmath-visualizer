#ifndef VMV_VMVWINDOW_H
#define VMV_VMVWINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace vmv
{
    class VMVWindow final
    {
      public:
        VMVWindow(int width, int height, std::string windowName);
        ~VMVWindow();

        VMVWindow(const VMVWindow&) = delete;
        VMVWindow(VMVWindow&&) noexcept = delete;
        VMVWindow& operator=(const VMVWindow&) = delete;
        VMVWindow& operator=(VMVWindow&&) noexcept = delete;

        bool ShouldClose();
        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
        VkExtent2D GetExtent();
        GLFWwindow* GetWindow() { return m_pWindow; }
        bool WasWindowResized() { return m_FrameBufferResized; }
        void ResetWindowResizedFlag() { m_FrameBufferResized = false; }

      private:
        static void FrameBufferResizeCallback(GLFWwindow* pWindow, int width, int height);

        void InitWindow();

        int m_Width;
        int m_Height;
        bool m_FrameBufferResized{false};

        std::string m_WindowName;
        GLFWwindow* m_pWindow{};
    };
} // namespace vmv

#endif