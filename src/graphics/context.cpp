#include <vkeng/graphics/base.hpp>

namespace vkeng::grph {

const auto REQUIRED_LAYER_NAMES = {
#ifndef NDEBUG
    "VK_LAYER_KHRONOS_validation",
#endif
};

const vk::ApplicationInfo APPLICATION_INFO = {
    .apiVersion = VK_API_VERSION_1_1,
};

vk::InstanceCreateInfo instanceCreateInfo(const App &app) {
  auto requiredExtensionNames = app.getRequiredInstanceExtensions();
  return vk::InstanceCreateInfo{.pApplicationInfo = &APPLICATION_INFO}
      .setPEnabledExtensionNames(requiredExtensionNames)
      .setPEnabledLayerNames(REQUIRED_LAYER_NAMES);
}

Context::Context(const App &app)
    : instance(context, instanceCreateInfo(app)), surface(app.createWindowSurface(instance)) {}

} // namespace vkeng::grph
