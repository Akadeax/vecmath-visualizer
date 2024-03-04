#include "VecmathVisualizer.h"

void vmv::VecmathVisualizer::run()
{
    while (!m_VMVWindow.ShouldClose())
    {
        glfwPollEvents();
    }
}