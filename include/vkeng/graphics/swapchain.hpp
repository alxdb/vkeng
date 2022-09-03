#pragma once

#include <vkeng/app.hpp>

#include <vulkan/vulkan_raii.hpp>

#include "base.hpp"

namespace vkeng::grph {

struct Swapchain {
  vk::SurfaceCapabilitiesKHR surfaceCapabilities;
  vk::Extent2D extent;
  vk::raii::SwapchainKHR handle;
  std::vector<vk::raii::ImageView> images;
  std::vector<vk::raii::Framebuffer> framebuffers;

  Swapchain(const App &,
            const Context &,
            const Device &device,
            const Pipeline &pipeline,
            const vk::SwapchainKHR & = VK_NULL_HANDLE);
};

} // namespace vkeng::grph
