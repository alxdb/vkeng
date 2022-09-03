#include <vkeng/vkeng.hpp>

#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace vkeng;

void updateUbo(const App &app, grph::Pipeline::UniformBufferObject &ubo) {
  float aspectRatio = app.getAspectRatio();
  ubo.model = glm::rotate(glm::mat4(1.0f), (float)0.0, glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::identity<glm::mat4>();
  ubo.proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
}

int main() {
  App app({.fullscreen = true});
  Graphics graphics(app);

  grph::DrawData quad = {
      .vertices =
          {
              {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
              {{+0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
              {{+0.5f, +0.5f}, {0.0f, 0.0f, 1.0f}},
              {{-0.5f, +0.5f}, {1.0f, 1.0f, 1.0f}},
          },
      .indices = {0, 1, 2, 2, 3, 0},
  };
  graphics.addDrawData(quad);

  grph::Pipeline::UniformBufferObject ubo;
  updateUbo(app, ubo);

  app.callbacks.onUpdate = [&](const App &app) {
    updateUbo(app, ubo);
    graphics.updateUbo(ubo);
    graphics.draw(app);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  };

  app.run();
}
