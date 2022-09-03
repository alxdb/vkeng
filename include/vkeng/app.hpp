#pragma once

#include <functional>
#include <optional>
#include <span>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <boost/core/noncopyable.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace vkeng {

struct Settings {
  int width = 1920;
  int height = 1080;
  const char *title = "vkeng";
  bool fullscreen = false;
};

class App;

struct Callbacks {
  std::function<void(const App &)> onUpdate = [](const App &) {};
  std::function<void(const App &, uint32_t, uint32_t)> onResize = [](const App &, uint32_t, uint32_t) {};
};

class App : private boost::noncopyable {
  GLFWwindow *window;

public:
  Callbacks callbacks;

  App(const Settings & = {});

  void run();
  ~App();

  vk::raii::SurfaceKHR createWindowSurface(const vk::raii::Instance &instance) const;
  std::span<const char *> getRequiredInstanceExtensions() const;
  std::array<uint32_t, 2> getFramebufferSize() const;
  float getAspectRatio() const;
};

} // namespace vkeng
