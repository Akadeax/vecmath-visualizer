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

      private:
        void InitWindow();

        const int m_Width;
        const int m_Height;

        std::string m_WindowName;
        GLFWwindow* m_pWindow;
    };
} // namespace vmv

#endif