#include "VMVWindow.h"

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

void vmv::VMVWindow::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_pWindow = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
}
