#include <vkeng/graphics/base.hpp>

#include <optional>
#include <ranges>
#include <unordered_set>

#include <vkeng/graphics/frame.hpp>

const std::array<const char *, 1> REQUIRED_DEVICE_EXTENSION_NAMES = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
const std::array<float, 1> QUEUE_PRIORITIES = {1.0};

vk::SurfaceFormatKHR pickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &formats) {
  auto preferredFormat = vk::SurfaceFormatKHR{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear};
  auto surfaceFormat = std::ranges::find(formats, preferredFormat);
  if (surfaceFormat != formats.end()) {
    return *surfaceFormat;
  } else {
    return formats.back();
  }
}

vk::PresentModeKHR pickPresentMode(const std::vector<vk::PresentModeKHR> &presentModes) {
  auto preferredMode = vk::PresentModeKHR::eMailbox;
  auto presentMode = std::ranges::find(presentModes, preferredMode);
  if (presentMode != presentModes.end()) {
    return *presentMode;
  } else {
    return vk::PresentModeKHR::eFifo;
  }
}

vk::raii::DescriptorPool createDescriptorPool(const vk::raii::Device &device) {
  auto poolSizes = {vk::DescriptorPoolSize{.descriptorCount = 2}};
  auto createInfo =
      vk::DescriptorPoolCreateInfo{
          .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
          .maxSets = 2,
      }
          .setPoolSizes(poolSizes);
  return device.createDescriptorPool(createInfo);
}

namespace vkeng::grph {

std::optional<Device::Details> isSuitable(const vk::raii::PhysicalDevice &physicalDevice,
                                          const vk::raii::SurfaceKHR &surface) {
  auto deviceExtensionsProperties = physicalDevice.enumerateDeviceExtensionProperties();

  // supports required extensions
  std::unordered_set<std::string> availableExtensionNames;
  std::ranges::transform(deviceExtensionsProperties,
                         std::inserter(availableExtensionNames, availableExtensionNames.begin()),
                         [](const auto &props) { return props.extensionName; });
  for (auto extensionName : REQUIRED_DEVICE_EXTENSION_NAMES) {
    if (!availableExtensionNames.contains(extensionName)) {
      return std::nullopt;
    }
  }

  // has a suitable queue family
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  auto queueFamily = std::ranges::find_if(queueFamilies, [](const auto &queueFamily) {
    return static_cast<bool>(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics);
  });
  if (queueFamily == queueFamilies.end()) {
    return std::nullopt;
  }
  uint32_t queueFamilyIndex = std::distance(queueFamilies.begin(), queueFamily);

  // supports surface
  auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
  auto presentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
  if (surfaceFormats.empty() || presentModes.empty() ||
      !physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, *surface)) {
    return std::nullopt;
  }

  return Device::Details{
      .physicalDevice = physicalDevice,
      .format = pickSurfaceFormat(surfaceFormats),
      .presentMode = pickPresentMode(presentModes),
      .queueFamilyIndex = queueFamilyIndex,
  };
}

Device::Details findSuitableDevice(const vk::raii::Instance &instance, const vk::raii::SurfaceKHR &surface) {
  for (const auto &physicalDevice : instance.enumeratePhysicalDevices()) {
    if (auto details = isSuitable(physicalDevice, surface)) {
      return *details;
    }
  }
  throw std::runtime_error("No suitable devices");
}

vk::raii::Device createDevice(const Device::Details &details) {
  auto queueCreateInfos = {
      vk::DeviceQueueCreateInfo{
          .queueFamilyIndex = details.queueFamilyIndex,
          .queueCount = 1,
      }
          .setQueuePriorities(QUEUE_PRIORITIES),
  };
  return {
      details.physicalDevice,
      vk::DeviceCreateInfo{}
          .setPEnabledExtensionNames(REQUIRED_DEVICE_EXTENSION_NAMES)
          .setQueueCreateInfos(queueCreateInfos),
  };
}

Device::Device(const Context &context)
    : details(findSuitableDevice(context.instance, context.surface)),
      handle(createDevice(details)),
      queue(handle.getQueue(details.queueFamilyIndex, 0)),
      commandPool(handle.createCommandPool({
          .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
          .queueFamilyIndex = details.queueFamilyIndex,
      })),
      descriptorPool(createDescriptorPool(handle)) {}

} // namespace vkeng::grph
