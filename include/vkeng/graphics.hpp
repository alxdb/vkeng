#pragma once

#include "graphics/base.hpp"
#include "graphics/buffers.hpp"
#include "graphics/drawdata.hpp"
#include "graphics/frame.hpp"
#include "graphics/swapchain.hpp"

namespace vkeng {

class Graphics {
  grph::Context context;
  grph::Device device;
  grph::Pipeline pipeline;
  grph::Swapchain swapchain;

  size_t currentFrameIndex = 0;
  grph::Frames frames;

  grph::Pipeline::UniformBufferObject ubo;
  std::vector<std::unique_ptr<grph::DrawBuffers>> drawBuffers;

public:
  Graphics(App &);
  ~Graphics();

  void updateUbo(grph::Pipeline::UniformBufferObject ubo);
  void addDrawData(const grph::DrawData &drawData);
  void draw(const App &);
};

} // namespace vkeng
