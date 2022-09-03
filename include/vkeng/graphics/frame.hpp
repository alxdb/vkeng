#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "base.hpp"
#include "buffers.hpp"

namespace vkeng::grph {

struct Frame {
  static const uint32_t FIF = 2;

  vk::raii::CommandBuffer commandBuffer;
  vk::raii::DescriptorSet descriptorSet;

  vk::raii::Semaphore imageAvailable;
  vk::raii::Semaphore renderFinished;
  vk::raii::Fence inFlight;

  buffers::HostBuffer<Pipeline::UniformBufferObject> uniformBuffer;

  Frame(vk::raii::CommandBuffer &&, vk::raii::DescriptorSet &&, const Device &);

  static std::array<Frame, FIF> createFrames(const Device &, const Pipeline &);
};

using Frames = std::array<Frame, Frame::FIF>;

} // namespace vkeng::grph
