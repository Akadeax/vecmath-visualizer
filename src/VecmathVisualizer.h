#ifndef VMV_VECMATHVISUALIZER_H
#define VMV_VECMATHVISUALIZER_H

#include "Core/VMVWindow.h"

namespace vmv
{
    class VecmathVisualizer
    {
      public:
        static constexpr int WIDTH{800};
        static constexpr int HEIGHT{600};

        void run();

      private:
        VMVWindow m_VMVWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    };
} // namespace vmv

#endif