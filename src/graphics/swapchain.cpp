#include <iostream>

#include <vkeng/graphics/swapchain.hpp>

vk::Extent2D determineExtent(const vkeng::App &app, const vk::SurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    auto [realWidth, realHeight] = app.getFramebufferSize();
    return {
        std::clamp(realWidth, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(realHeight, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
    };
  }
}

uint32_t determineMinImageCount(const vk::SurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.maxImageCount == 0) {
    return capabilities.minImageCount + 1;
  } else {
    return capabilities.maxImageCount;
  }
}

std::vector<vk::raii::ImageView> createImages(const vk::raii::SwapchainKHR &swapchain,
                                              const vk::raii::Device &device,
                                              const vk::Format &format) {
  auto swapchainImages = swapchain.getImages();

  std::vector<vk::raii::ImageView> result;
  result.reserve(swapchainImages.size());

  std::ranges::transform(swapchainImages, std::back_inserter(result), [&](const auto &image) {
    return device.createImageView(vk::ImageViewCreateInfo{
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    });
  });

  return result;
}

std::vector<vk::raii::Framebuffer> createFramebuffers(const vk::raii::RenderPass &renderPass,
                                                      const vk::raii::Device &device,
                                                      const std::vector<vk::raii::ImageView> &images,
                                                      const vk::Extent2D &extent) {
  std::vector<vk::raii::Framebuffer> result;
  result.reserve(images.size());

  std::ranges::transform(images, std::back_inserter(result), [&](const auto &view) {
    std::array<vk::ImageView, 1> attachments = {*view};

    auto createInfo =
        vk::FramebufferCreateInfo{
            .renderPass = *renderPass,
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        }
            .setAttachments(attachments);

    return device.createFramebuffer(createInfo);
  });

  return result;
}

namespace vkeng::grph {

Swapchain::Swapchain(const App &app,
                     const Context &context,
                     const Device &device,
                     const Pipeline &pipeline,
                     const vk::SwapchainKHR &swapchain)
    : surfaceCapabilities(device.details.physicalDevice.getSurfaceCapabilitiesKHR(*context.surface)),
      extent(determineExtent(app, surfaceCapabilities)),
      handle(device.handle.createSwapchainKHR({
          .surface = *context.surface,
          .minImageCount = determineMinImageCount(surfaceCapabilities),
          .imageFormat = device.details.format.format,
          .imageColorSpace = device.details.format.colorSpace,
          .imageExtent = extent,
          .imageArrayLayers = 1,
          .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
          .imageSharingMode = vk::SharingMode::eExclusive,
          .preTransform = surfaceCapabilities.currentTransform,
          .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
          .presentMode = device.details.presentMode,
          .clipped = true,
          .oldSwapchain = swapchain,
      })),
      images(createImages(handle, device.handle, device.details.format.format)),
      framebuffers(createFramebuffers(pipeline.renderPass, device.handle, images, extent)) {}

} // namespace vkeng::grph
