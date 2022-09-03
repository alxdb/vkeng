#include <vkeng/graphics/drawdata.hpp>

namespace vkeng::grph {

DrawBuffers::DrawBuffers(const Device &device, const DrawData &drawdata)
    : vertexBuffer(device, drawdata.vertices, vk::BufferUsageFlagBits::eVertexBuffer),
      indexBuffer(device, drawdata.indices, vk::BufferUsageFlagBits::eIndexBuffer) {}

void DrawBuffers::copyData(const Device &device) const {
  vertexBuffer.copyData(device);
  indexBuffer.copyData(device);
}

} // namespace vkeng::grph
