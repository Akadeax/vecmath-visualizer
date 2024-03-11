# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/bin/Debug/Shaders")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

set(SOURCES
    "main.cpp"
    "VecmathVisualizer.h" "VecmathVisualizer.cpp"
    "KeyboardMovementController.h" "KeyboardMovementController.cpp"
    "Core/VMVWindow.h" "Core/VMVWindow.cpp"
    "Core/VMVPipeline.h" "Core/VMVPipeline.cpp"
    "Core/VMVDevice.h" "Core/VMVDevice.cpp"
    "Core/VMVSwapChain.h" "Core/VMVSwapChain.cpp"
    "Core/VMVModel.h" "Core/VMVModel.cpp"
    "Core/VMVGameObject.h" "Core/VMVGameObject.cpp"
    "Core/VMVRenderer.h" "Core/VMVRenderer.cpp"
    "Core/SimpleRenderSystem.h" "Core/SimpleRenderSystem.cpp"
    "Core/VMVCamera.h" "Core/VMVCamera.cpp"
    "Core/VMVUtils.h"
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} Shaders)

set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_WARNING_AS_ERROR ON)

# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)
