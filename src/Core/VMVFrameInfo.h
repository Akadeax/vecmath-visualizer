#ifndef VMV_VMVFRAMEINFO_H
#define VMV_VMVFRAMEINFO_H

#include "VMVCamera.h"
#include <vulkan/vulkan.h>

namespace vmv
{
    struct VMVFrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        VMVCamera& camera;
    };
} // namespace vmv

#endif