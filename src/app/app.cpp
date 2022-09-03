#include <iostream>
#include <mutex>
#include <stdexcept>

#include <spdlog/spdlog.h>

#include <vkeng/app.hpp>

std::once_flag glfw_initialized;

void initialize_glfw() {
  glfwSetErrorCallback([](int code, const char *message) { spdlog::error("GLFW Error [{}]: {}", code, message); });

  if (glfwInit() != GLFW_TRUE) {
    throw std::runtime_error("Couldn't initialize GLFW");
  }
  if (std::atexit(glfwTerminate) != 0) {
    throw std::runtime_error("Couldn't register GLFW termination");
  }
}

namespace vkeng {

App::App(const Settings &settings) {
  std::call_once(glfw_initialized, initialize_glfw);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  if (settings.fullscreen) {
    auto monitor = glfwGetPrimaryMonitor();
    auto vd_mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_REFRESH_RATE, vd_mode->refreshRate);
    glfwWindowHint(GLFW_RED_BITS, vd_mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, vd_mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, vd_mode->blueBits);
    window = glfwCreateWindow(vd_mode->width, vd_mode->height, settings.title, monitor, nullptr);
  } else {
    window = glfwCreateWindow(settings.height, settings.height, settings.title, nullptr, nullptr);
  }
  glfwSetWindowUserPointer(window, this);

  glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    app->callbacks.onResize(*app, width, height);
  });
}

App::~App() { glfwDestroyWindow(window); }

void App::run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    callbacks.onUpdate(*this);
  }
}

vk::raii::SurfaceKHR App::createWindowSurface(const vk::raii::Instance &instance) const {
  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS) {
    throw std::runtime_error("Couldn't create window surface");
  }
  return {instance, surface};
}

std::span<const char *> App::getRequiredInstanceExtensions() const {
  uint32_t count;
  const char **ptr = glfwGetRequiredInstanceExtensions(&count);
  return {ptr, count};
}

std::array<uint32_t, 2> App::getFramebufferSize() const {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  return {(uint32_t)width, (uint32_t)height};
}

float App::getAspectRatio() const {
  auto [width, height] = getFramebufferSize();
  return (float)width / (float)height;
}

} // namespace vkeng
