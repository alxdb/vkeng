find_package(Vulkan REQUIRED COMPONENTS glslc)
find_package(glm REQUIRED)

add_subdirectory(shaders)

add_library(vkeng-grph context.cpp device.cpp pipeline.cpp swapchain.cpp frame.cpp buffers.cpp drawdata.cpp graphics.cpp)

target_link_libraries(vkeng-grph PRIVATE Vulkan::Vulkan shaders spdlog fmt)
target_include_directories(vkeng-grph PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(vkeng-grph PUBLIC VULKAN_HPP_NO_CONSTRUCTORS SPDLOG_FMT_EXTERNAL)
