#pragma once

#include "base.hpp"
#include "buffers.hpp"

namespace vkeng::grph {

using VertexBuffer = buffers::StagedBuffer<std::vector<Pipeline::Vertex>>;
using IndexBuffer = buffers::StagedBuffer<std::vector<Pipeline::Index>>;

struct DrawData {
  const std::vector<Pipeline::Vertex> vertices;
  const std::vector<Pipeline::Index> indices;
};

struct DrawBuffers {
  const VertexBuffer vertexBuffer;
  const IndexBuffer indexBuffer;

  DrawBuffers(const Device &, const DrawData &);

  void copyData(const Device &) const;
};

} // namespace vkeng::grph
