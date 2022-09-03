#include <vkeng/graphics.hpp>

#include <spdlog/spdlog.h>

namespace vkeng {

Graphics::Graphics(App &app)
    : context(app),
      device(context),
      pipeline(device),
      swapchain(app, context, device, pipeline),
      frames(grph::Frame::createFrames(device, pipeline)) {
  app.callbacks.onResize = [&](const App &app, uint32_t, uint32_t) {
    device.handle.waitIdle();
    swapchain = grph::Swapchain(app, context, device, pipeline, *swapchain.handle);
  };
}

Graphics::~Graphics() { device.handle.waitIdle(); }

void Graphics::updateUbo(grph::Pipeline::UniformBufferObject ubo) { this->ubo = std::move(ubo); }
void Graphics::addDrawData(const grph::DrawData &drawData) {
  drawBuffers.push_back(std::make_unique<grph::DrawBuffers>(device, drawData));
}

void Graphics::draw(const App &app) {

  const grph::Frame &currentFrame = frames[currentFrameIndex];
  currentFrame.uniformBuffer.copyData(ubo);

  auto fenceResult = device.handle.waitForFences({*currentFrame.inFlight}, true, std::numeric_limits<uint64_t>::max());
  if (fenceResult != vk::Result::eSuccess) {
    throw std::runtime_error("Failed waiting for fence");
  }

  auto [acquireResult, imageIndex] =
      swapchain.handle.acquireNextImage(std::numeric_limits<uint64_t>::max(), *currentFrame.imageAvailable);

  if (acquireResult == vk::Result::eErrorOutOfDateKHR) {
    spdlog::info("Image Out Of Date");
    device.handle.waitIdle();
    swapchain = grph::Swapchain(app, context, device, pipeline, *swapchain.handle);
    return;
  } else if (acquireResult != vk::Result::eSuccess && acquireResult != vk::Result::eSuboptimalKHR) {
    throw std::runtime_error("Failed acquire");
  }

  // must occur after swapchain recreation, due to early return
  device.handle.resetFences({*currentFrame.inFlight});

  currentFrame.commandBuffer.reset();

  // record command buffer
  const auto &commandBuffer = currentFrame.commandBuffer;
  std::array<vk::ClearValue, 1> clearValues = {{{{{{0.0f, 0.0f, 0.0f, 1.0f}}}}}};
  std::array<vk::Viewport, 1> viewports = {vk::Viewport{
      .x = 0.0f,
      .y = 0.0f,
      .width = static_cast<float>(swapchain.extent.width),
      .height = static_cast<float>(swapchain.extent.height),
      .minDepth = 0.0f,
      .maxDepth = 1.0f,
  }};
  std::array<vk::Rect2D, 1> scissors = {vk::Rect2D{
      .offset = {0, 0},
      .extent = swapchain.extent,
  }};

  commandBuffer.begin({});
  commandBuffer.beginRenderPass(
      vk::RenderPassBeginInfo{
          .renderPass = *pipeline.renderPass,
          .framebuffer = *swapchain.framebuffers[imageIndex],
          .renderArea =
              {
                  .offset = {0, 0},
                  .extent = swapchain.extent,
              },
      }
          .setClearValues(clearValues),
      vk::SubpassContents::eInline);
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.handle);
  for (const auto &drawBuffer : drawBuffers) {
    commandBuffer.bindVertexBuffers(0, {*drawBuffer->vertexBuffer.deviceBuffer.buffer}, {0});
    commandBuffer.bindIndexBuffer(*drawBuffer->indexBuffer.deviceBuffer.buffer, 0, vk::IndexType::eUint16);
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, *pipeline.pipelineLayout, 0, {*frames[currentFrameIndex].descriptorSet}, {});
    commandBuffer.setViewport(0, viewports);
    commandBuffer.setScissor(0, scissors);
    commandBuffer.drawIndexed(drawBuffer->indexBuffer.stagingBuffer.data.size(), 1, 0, 0, 0);
  }
  commandBuffer.endRenderPass();
  commandBuffer.end();

  auto waitStages = {static_cast<vk::PipelineStageFlags>(vk::PipelineStageFlagBits::eColorAttachmentOutput)};
  device.queue.submit(vk::SubmitInfo{}
                          .setWaitSemaphores(*currentFrame.imageAvailable)
                          .setWaitDstStageMask(waitStages)
                          .setCommandBuffers(*currentFrame.commandBuffer)
                          .setSignalSemaphores(*currentFrame.renderFinished),
                      *currentFrame.inFlight);

  auto swapchains = {*swapchain.handle};
  auto imageIndices = {imageIndex};
  auto presentResult = device.queue.presentKHR(vk::PresentInfoKHR{}
                                                   .setWaitSemaphores(*currentFrame.renderFinished)
                                                   .setSwapchains(swapchains)
                                                   .setImageIndices(imageIndices));
  if (presentResult == vk::Result::eErrorOutOfDateKHR) {
    spdlog::info("Image Out Of Date");
    device.handle.waitIdle();
    swapchain = grph::Swapchain(app, context, device, pipeline, *swapchain.handle);
  } else if (presentResult != vk::Result::eSuccess && presentResult != vk::Result::eSuboptimalKHR) {
    throw std::runtime_error("Failed presentation");
  }

  currentFrameIndex = currentFrameIndex ^ 1;
}

}; // namespace vkeng
