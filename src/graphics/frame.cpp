#include <vkeng/graphics/frame.hpp>

namespace vkeng::grph {

Frame::Frame(vk::raii::CommandBuffer &&commandBuffer, vk::raii::DescriptorSet &&descriptorSet, const Device &device)
    : commandBuffer(std::move(commandBuffer)),
      descriptorSet(std::move(descriptorSet)),
      imageAvailable(device.handle.createSemaphore({})),
      renderFinished(device.handle.createSemaphore({})),
      inFlight(device.handle.createFence({.flags = vk::FenceCreateFlagBits::eSignaled})),
      uniformBuffer(device, vk::BufferUsageFlagBits::eUniformBuffer) {
  auto descriptorBufferInfo = {vk::DescriptorBufferInfo{
      .buffer = *uniformBuffer.buffer,
      .offset = 0,
      .range = sizeof(Pipeline::UniformBufferObject),
  }};
  auto writeDescriptorSet =
      vk::WriteDescriptorSet{
          .dstSet = *this->descriptorSet,
          .dstBinding = 0,
          .dstArrayElement = 0,
          .descriptorType = vk::DescriptorType::eUniformBuffer,
      }
          .setBufferInfo(descriptorBufferInfo);
  device.handle.updateDescriptorSets({writeDescriptorSet}, {});
}

std::array<Frame, 2> Frame::createFrames(const Device &device, const Pipeline &pipeline) {
  auto commandBuffers = device.handle.allocateCommandBuffers({
      .commandPool = *device.commandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 2,
  });
  auto descriptorSetLayouts = {
      *pipeline.descriptorSetLayout,
      *pipeline.descriptorSetLayout,
  };

  auto descriptorSetAllocateInfo =
      vk::DescriptorSetAllocateInfo{
          .descriptorPool = *device.descriptorPool,
      }
          .setSetLayouts(descriptorSetLayouts);
  auto descriptorSets = device.handle.allocateDescriptorSets(descriptorSetAllocateInfo);

  return {Frame(std::move(commandBuffers[0]), std::move(descriptorSets[0]), device),
          Frame(std::move(commandBuffers[1]), std::move(descriptorSets[1]), device)};
}

} // namespace vkeng::grph
