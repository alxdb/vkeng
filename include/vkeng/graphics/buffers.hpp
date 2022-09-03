#pragma once

#include <ranges>

#include <vulkan/vulkan_raii.hpp>

#include "base.hpp"

namespace vkeng::grph::buffers {

struct Buffer {
  const size_t size;
  const vk::raii::Buffer buffer;
  const vk::raii::DeviceMemory memory;

  Buffer(const Device &, size_t, vk::BufferUsageFlags, vk::MemoryPropertyFlags);
};

template <std::ranges::contiguous_range R>
struct ContiguousHostBuffer : Buffer {
  const R data;

  ContiguousHostBuffer(const Device &, const R &, vk::BufferUsageFlags);

  void copyData() const;
};

template <typename T>
struct HostBuffer : Buffer {
  HostBuffer(const Device &, vk::BufferUsageFlags);

  void copyData(const T &data) const;
};

template <std::ranges::contiguous_range R>
struct StagedBuffer {
  const ContiguousHostBuffer<R> stagingBuffer;
  const Buffer deviceBuffer;

  StagedBuffer(const Device &, const R &, vk::BufferUsageFlags);

  void copyData(const Device &) const;
};

// Generic Implementation

template <std::ranges::contiguous_range R>
ContiguousHostBuffer<R>::ContiguousHostBuffer(const Device &device, const R &data, vk::BufferUsageFlags usage)
    : Buffer(device,
             sizeof(std::ranges::range_value_t<R>) * std::ranges::size(data),
             usage,
             vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible),
      data(data) {}

template <std::ranges::contiguous_range R>
void ContiguousHostBuffer<R>::copyData() const {
  void *mappedMemory = this->memory.mapMemory(0, this->size);
  memcpy(mappedMemory, std::ranges::data(this->data), this->size);
  this->memory.unmapMemory();
}

template <typename T>
HostBuffer<T>::HostBuffer(const Device &device, vk::BufferUsageFlags usage)
    : Buffer(device,
             sizeof(T),
             usage,
             vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible) {}

template <typename T>
void HostBuffer<T>::copyData(const T &data) const {
  void *mappedMemory = this->memory.mapMemory(0, this->size);
  memcpy(mappedMemory, &data, this->size);
  this->memory.unmapMemory();
}

template <std::ranges::contiguous_range R>
StagedBuffer<R>::StagedBuffer(const Device &device, const R &data, vk::BufferUsageFlags usage)
    : stagingBuffer(device, data, vk::BufferUsageFlagBits::eTransferSrc),
      deviceBuffer(device,
                   stagingBuffer.size,
                   vk::BufferUsageFlagBits::eTransferDst | usage,
                   vk::MemoryPropertyFlagBits::eDeviceLocal) {}

template <std::ranges::contiguous_range R>
void StagedBuffer<R>::copyData(const Device &device) const {
  stagingBuffer.copyData();

  auto commandBuffer = std::move(device.handle.allocateCommandBuffers({
      .commandPool = *device.commandPool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = 1,
  })[0]);

  commandBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  commandBuffer.copyBuffer(*stagingBuffer.buffer, *deviceBuffer.buffer, {{.size = stagingBuffer.size}});
  commandBuffer.end();

  auto commandBuffers = {*commandBuffer};
  device.queue.submit({vk::SubmitInfo{}.setCommandBuffers(commandBuffers)});
  device.queue.waitIdle();
}

} // namespace vkeng::grph::buffers
