#include "VMVWindow.h"
#include <stdexcept>

vmv::VMVWindow::VMVWindow(int width, int height, std::string windowName)
    : m_Width{width}, m_Height{height}, m_WindowName{std::move(windowName)}
{
    InitWindow();
}

vmv::VMVWindow::~VMVWindow()
{
    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

bool vmv::VMVWindow::ShouldClose()
{
    return glfwWindowShouldClose(m_pWindow);
}

void vmv::VMVWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

VkExtent2D vmv::VMVWindow::GetExtent()
{
    return {static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height)};
}

void vmv::VMVWindow::FrameBufferResizeCallback(GLFWwindow* pWindow, int width, int height)
{
    VMVWindow* pVMVWindow{reinterpret_cast<VMVWindow*>(glfwGetWindowUserPointer(pWindow))};
    pVMVWindow->m_FrameBufferResized = true;
    pVMVWindow->m_Width = width;
    pVMVWindow->m_Height = height;
}

void vmv::VMVWindow::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_pWindow = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetFramebufferSizeCallback(m_pWindow, FrameBufferResizeCallback);
}
