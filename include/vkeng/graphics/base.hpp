#pragma once

#include <vkeng/app.hpp>

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace vkeng::grph {

struct Context {
  vk::raii::Context context;
  vk::raii::Instance instance;
  vk::raii::SurfaceKHR surface;

  Context(const App &);
};

struct Device {
  struct Details {
    vk::raii::PhysicalDevice physicalDevice;

    vk::SurfaceFormatKHR format;
    vk::PresentModeKHR presentMode;

    uint32_t queueFamilyIndex;
  } details;

  vk::raii::Device handle;
  vk::raii::Queue queue;
  vk::raii::CommandPool commandPool;
  vk::raii::DescriptorPool descriptorPool;

  Device(const Context &);
};

struct Pipeline {
  using Index = uint16_t;

  struct Vertex {
    static vk::VertexInputBindingDescription bindingDescription;
    static vk::VertexInputAttributeDescription attributeDescriptions[2];

    glm::vec2 pos;
    glm::vec3 color;
  };

  struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
  };

  vk::raii::DescriptorSetLayout descriptorSetLayout;
  vk::raii::PipelineLayout pipelineLayout;
  vk::raii::RenderPass renderPass;
  vk::raii::Pipeline handle;

  Pipeline(const Device &);
};

} // namespace vkeng::grph
