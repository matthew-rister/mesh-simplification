#include "engine/engine.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <ranges>
#include <utility>

#include "engine/mesh.h"
#include "engine/shader_module.h"
#include "engine/window.h"

namespace {

vk::UniqueRenderPass CreateRenderPass(const vk::Device& device, const gfx::Swapchain& swapchain) {
  const vk::AttachmentDescription attachment_description{.format = swapchain.image_format(),
                                                         .samples = vk::SampleCountFlagBits::e1,
                                                         .loadOp = vk::AttachmentLoadOp::eClear,
                                                         .storeOp = vk::AttachmentStoreOp::eStore,
                                                         .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                                                         .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                                         .initialLayout = vk::ImageLayout::eUndefined,
                                                         .finalLayout = vk::ImageLayout::ePresentSrcKHR};

  static constexpr vk::AttachmentReference kColorAttachmentReference{
      .attachment = 0,
      .layout = vk::ImageLayout::eColorAttachmentOptimal};

  static constexpr vk::SubpassDescription kSubpassDescription{.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
                                                              .colorAttachmentCount = 1,
                                                              .pColorAttachments = &kColorAttachmentReference};

  return device.createRenderPassUnique(vk::RenderPassCreateInfo{.attachmentCount = 1,
                                                                .pAttachments = &attachment_description,
                                                                .subpassCount = 1,
                                                                .pSubpasses = &kSubpassDescription});
}

std::vector<vk::UniqueFramebuffer> CreateFramebuffers(const vk::Device& device,
                                                      const gfx::Swapchain& swapchain,
                                                      const vk::RenderPass& render_pass) {
  return swapchain.image_views()
         | std::views::transform([&device, &render_pass, &extent = swapchain.image_extent()](const auto& image_view) {
             return device.createFramebufferUnique(vk::FramebufferCreateInfo{.renderPass = render_pass,
                                                                             .attachmentCount = 1,
                                                                             .pAttachments = &image_view,
                                                                             .width = extent.width,
                                                                             .height = extent.height,
                                                                             .layers = 1});
           })
         | std::ranges::to<std::vector>();
}

vk::UniquePipeline CreateGraphicsPipeline(const vk::Device& device,
                                          const gfx::Swapchain& swapchain,
                                          const vk::PipelineLayout& pipeline_layout,
                                          const vk::RenderPass& render_pass) {
  const gfx::ShaderModule vertex_shader_module{device, vk::ShaderStageFlagBits::eVertex, "shaders/vertex.glsl"};
  const gfx::ShaderModule fragment_shader_module{device, vk::ShaderStageFlagBits::eFragment, "shaders/fragment.glsl"};

  const std::array shader_stage_create_info{
      vk::PipelineShaderStageCreateInfo{.stage = vk::ShaderStageFlagBits::eVertex,
                                        .module = *vertex_shader_module,
                                        .pName = "main"},
      vk::PipelineShaderStageCreateInfo{.stage = vk::ShaderStageFlagBits::eFragment,
                                        .module = *fragment_shader_module,
                                        .pName = "main"}};

  static constexpr vk::PipelineVertexInputStateCreateInfo kVertexInputStateCreateInfo{
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &gfx::Mesh::kVertexInputBindingDescription,
      .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(gfx::Mesh::kVertexAttributeDescriptions.size()),
      .pVertexAttributeDescriptions = gfx::Mesh::kVertexAttributeDescriptions.data()};

  static constexpr vk::PipelineInputAssemblyStateCreateInfo kInputAssemblyStateCreateInfo{
      .topology = vk::PrimitiveTopology::eTriangleList};

  const auto& swapchain_image_extent = swapchain.image_extent();
  const vk::Viewport viewport{.x = 0.0f,
                              .y = 0.0f,
                              .width = static_cast<float>(swapchain_image_extent.width),
                              .height = static_cast<float>(swapchain_image_extent.height),
                              .minDepth = 0.0f,
                              .maxDepth = 1.0f};
  const vk::Rect2D scissor{.offset = vk::Offset2D{0, 0}, .extent = swapchain_image_extent};

  const vk::PipelineViewportStateCreateInfo viewport_state_create_info{.viewportCount = 1,
                                                                       .pViewports = &viewport,
                                                                       .scissorCount = 1,
                                                                       .pScissors = &scissor};

  static constexpr vk::PipelineRasterizationStateCreateInfo kRasterizationStateCreateInfo{
      .polygonMode = vk::PolygonMode::eFill,
      .cullMode = vk::CullModeFlagBits::eBack,
      .frontFace = vk::FrontFace::eCounterClockwise,
      .lineWidth = 1.0f};

  static constexpr vk::PipelineMultisampleStateCreateInfo kMultisampleStateCreateInfo{.rasterizationSamples =
                                                                                          vk::SampleCountFlagBits::e1};

  static constexpr vk::PipelineColorBlendAttachmentState kColorBlendAttachmentState{
      .blendEnable = vk::True,
      .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
      .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
      .colorBlendOp = vk::BlendOp::eAdd,
      .srcAlphaBlendFactor = vk::BlendFactor::eOne,
      .dstAlphaBlendFactor = vk::BlendFactor::eZero,
      .alphaBlendOp = vk::BlendOp::eAdd,
      .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB
                        | vk::ColorComponentFlagBits::eA};

  static constexpr vk::PipelineColorBlendStateCreateInfo kColorBlendStateCreateInfo{
      .attachmentCount = 1,
      .pAttachments = &kColorBlendAttachmentState,
      .blendConstants = std::array{0.0f, 0.0f, 0.0f, 0.0f}};

  auto [result, graphics_pipeline] = device.createGraphicsPipelineUnique(
      nullptr,
      vk::GraphicsPipelineCreateInfo{.stageCount = static_cast<std::uint32_t>(shader_stage_create_info.size()),
                                     .pStages = shader_stage_create_info.data(),
                                     .pVertexInputState = &kVertexInputStateCreateInfo,
                                     .pInputAssemblyState = &kInputAssemblyStateCreateInfo,
                                     .pViewportState = &viewport_state_create_info,
                                     .pRasterizationState = &kRasterizationStateCreateInfo,
                                     .pMultisampleState = &kMultisampleStateCreateInfo,
                                     .pColorBlendState = &kColorBlendStateCreateInfo,
                                     .layout = pipeline_layout,
                                     .renderPass = render_pass,
                                     .subpass = 0});
  vk::resultCheck(result,
                  std::format("Graphics pipeline creation failed with error {}", vk::to_string(result)).c_str());

  return std::move(graphics_pipeline);  // return value optimization not available here
}

vk::UniqueCommandPool CreateCommandPool(const gfx::Device& device) {
  return device->createCommandPoolUnique(
      vk::CommandPoolCreateInfo{.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                .queueFamilyIndex = device.graphics_queue().queue_family_index()});
}

template <std::size_t N>
std::vector<vk::UniqueCommandBuffer> AllocateCommandBuffers(const vk::Device& device,
                                                            const vk::CommandPool& command_pool) {
  return device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{
      .commandPool = command_pool,
      .level = vk::CommandBufferLevel::ePrimary,
      .commandBufferCount = N,
  });
}

template <std::size_t N>
std::array<vk::UniqueSemaphore, N> CreateSemaphores(const vk::Device& device) {
  std::array<vk::UniqueSemaphore, N> semaphores;
  std::ranges::generate(semaphores, [&device] {
    return device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
  });
  return semaphores;
}

template <std::size_t N>
std::array<vk::UniqueFence, N> CreateFences(const vk::Device& device) {
  std::array<vk::UniqueFence, N> fences;
  std::ranges::generate(fences, [&device] {
    return device.createFenceUnique(vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
  });
  return fences;
}

}  // namespace

gfx::Engine::Engine(const Window& window)
    : surface_{window.CreateSurface(*instance_)},
      device_{*instance_, *surface_},
      swapchain_{device_, window, *surface_},
      mesh_{device_},
      render_pass_{CreateRenderPass(*device_, swapchain_)},
      framebuffers_{CreateFramebuffers(*device_, swapchain_, *render_pass_)},
      graphics_pipeline_layout_{device_->createPipelineLayoutUnique(vk::PipelineLayoutCreateInfo{})},
      graphics_pipeline_{CreateGraphicsPipeline(*device_, swapchain_, *graphics_pipeline_layout_, *render_pass_)},
      command_pool_{CreateCommandPool(device_)},
      command_buffers_{AllocateCommandBuffers<kMaxRenderFrames>(*device_, *command_pool_)},
      acquire_next_image_semaphores_{CreateSemaphores<kMaxRenderFrames>(*device_)},
      present_image_semaphores_{CreateSemaphores<kMaxRenderFrames>(*device_)},
      draw_fences_{CreateFences<kMaxRenderFrames>(*device_)} {}

gfx::Engine::~Engine() noexcept {
  try {
    device_->waitIdle();
  } catch (const vk::SystemError& error) {
    std::cerr << error.what() << std::endl;
  }
}

void gfx::Engine::Render() {
  current_frame_index_ = (current_frame_index_ + 1) % kMaxRenderFrames;
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index): index verified prior to array access
  const auto& command_buffer = *command_buffers_[current_frame_index_];
  const auto& acquire_next_image_semaphore = *acquire_next_image_semaphores_[current_frame_index_];
  const auto& present_image_semaphore = *present_image_semaphores_[current_frame_index_];
  const auto& draw_fence = *draw_fences_[current_frame_index_];
  // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)

  static constexpr auto kMaxTimeout = std::numeric_limits<std::uint64_t>::max();
  auto result = device_->waitForFences(draw_fence, vk::True, kMaxTimeout);
  vk::resultCheck(result, std::format("vkWaitForFences failed with result {}", vk::to_string(result)).c_str());
  device_->resetFences(draw_fence);

  std::uint32_t image_index{};
  std::tie(result, image_index) = device_->acquireNextImageKHR(*swapchain_, kMaxTimeout, acquire_next_image_semaphore);
  vk::resultCheck(result, std::format("vkAcquireNextImageKHR failed with error {}", vk::to_string(result)).c_str());

  command_buffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

  static constexpr vk::ClearValue kClearValue{.color = vk::ClearColorValue{std::array{0.0f, 0.0f, 0.0f, 1.0f}}};
  command_buffer.beginRenderPass(vk::RenderPassBeginInfo{.renderPass = *render_pass_,
                                                         .framebuffer = *framebuffers_[image_index],
                                                         .renderArea = vk::Rect2D{.offset = vk::Offset2D{0, 0},
                                                                                  .extent = swapchain_.image_extent()},
                                                         .clearValueCount = 1,
                                                         .pClearValues = &kClearValue},
                                 vk::SubpassContents::eInline);

  command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphics_pipeline_);
  mesh_.Render(command_buffer);

  command_buffer.endRenderPass();
  command_buffer.end();

  static constexpr vk::PipelineStageFlags kWaitPipelineStage = vk::PipelineStageFlagBits::eTopOfPipe;
  device_.graphics_queue()->submit(vk::SubmitInfo{.waitSemaphoreCount = 1,
                                                  .pWaitSemaphores = &acquire_next_image_semaphore,
                                                  .pWaitDstStageMask = &kWaitPipelineStage,
                                                  .commandBufferCount = 1,
                                                  .pCommandBuffers = &command_buffer,
                                                  .signalSemaphoreCount = 1,
                                                  .pSignalSemaphores = &present_image_semaphore},
                                   draw_fence);

  result = device_.present_queue()->presentKHR(vk::PresentInfoKHR{.waitSemaphoreCount = 1,
                                                                  .pWaitSemaphores = &present_image_semaphore,
                                                                  .swapchainCount = 1,
                                                                  .pSwapchains = &(*swapchain_),
                                                                  .pImageIndices = &image_index});
  vk::resultCheck(result, std::format("vkQueuePresentKHR failed with error {}", vk::to_string(result)).c_str());
}
