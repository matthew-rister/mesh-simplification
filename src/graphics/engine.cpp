#include "graphics/engine.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <limits>
#include <ranges>
#include <utility>

#include <glm/mat4x4.hpp>

#include "graphics/arc_camera.h"
#include "graphics/mesh.h"
#include "graphics/shader_module.h"
#include "graphics/window.h"

namespace {

struct VertexTransforms {
  glm::mat4 model_view_transform{1.0f};
  glm::mat4 projection_transform{1.0f};
};

vk::SampleCountFlagBits GetMsaaSampleCount(const vk::PhysicalDeviceLimits& physical_device_limits) {
  const auto color_sample_count_flags = physical_device_limits.framebufferColorSampleCounts;
  const auto depth_sample_count_flags = physical_device_limits.framebufferDepthSampleCounts;
  const auto color_depth_sample_count_flags = color_sample_count_flags & depth_sample_count_flags;

  using enum vk::SampleCountFlagBits;
  for (const auto msaa_sample_count_bit : {e8, e4, e2}) {
    if (msaa_sample_count_bit & color_depth_sample_count_flags) {
      return msaa_sample_count_bit;
    }
  }

  assert(color_depth_sample_count_flags & e1);
  return e1;  // multisample anti-aliasing is not supported on this device
}

vk::Format GetDepthAttachmentFormat(const vk::PhysicalDevice physical_device) {
  // the Vulkan specification requires VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT support for VK_FORMAT_D16_UNORM
  // and at least one of VK_FORMAT_X8_D24_UNORM_PACK32 and VK_FORMAT_D32_SFLOAT
  using enum vk::Format;
  for (const auto depth_attachment_format : {eD32Sfloat, eX8D24UnormPack32}) {
    const auto format_properties = physical_device.getFormatProperties(depth_attachment_format);
    if (format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
      return depth_attachment_format;
    }
  }

#ifndef NDEBUG
  const auto d16_unorm_format_properties = physical_device.getFormatProperties(eD16Unorm);
  assert(d16_unorm_format_properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment);
#endif
  return eD16Unorm;
}

vk::UniqueRenderPass CreateRenderPass(const vk::Device device,
                                      const vk::SampleCountFlagBits msaa_sample_count,
                                      const vk::Format color_attachment_format,
                                      const vk::Format depth_attachment_format) {
  const vk::AttachmentDescription color_attachment_description{.format = color_attachment_format,
                                                               .samples = msaa_sample_count,
                                                               .loadOp = vk::AttachmentLoadOp::eClear,
                                                               .storeOp = vk::AttachmentStoreOp::eDontCare,
                                                               .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                                                               .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                                               .initialLayout = vk::ImageLayout::eUndefined,
                                                               .finalLayout = vk::ImageLayout::eColorAttachmentOptimal};

  const vk::AttachmentDescription color_resolve_attachment_description{
      .format = color_attachment_format,
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::ePresentSrcKHR};

  const vk::AttachmentDescription depth_resolve_attachment_description{
      .format = depth_attachment_format,
      .samples = msaa_sample_count,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eDontCare,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal};

  const std::array attachment_descriptions{color_attachment_description,
                                           color_resolve_attachment_description,
                                           depth_resolve_attachment_description};

  static constexpr vk::AttachmentReference kColorAttachmentReference{
      .attachment = 0,
      .layout = vk::ImageLayout::eColorAttachmentOptimal};

  static constexpr vk::AttachmentReference kColorResolveAttachmentReference{
      .attachment = 1,
      .layout = vk::ImageLayout::eColorAttachmentOptimal};

  static constexpr vk::AttachmentReference kDepthAttachmentReference{
      .attachment = 2,
      .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal};

  static constexpr vk::SubpassDescription kSubpassDescription{.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
                                                              .colorAttachmentCount = 1,
                                                              .pColorAttachments = &kColorAttachmentReference,
                                                              .pResolveAttachments = &kColorResolveAttachmentReference,
                                                              .pDepthStencilAttachment = &kDepthAttachmentReference};

  static constexpr vk::SubpassDependency kSubpassDependency{
      .srcSubpass = vk::SubpassExternal,
      .dstSubpass = 0,
      .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
      .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
      .srcAccessMask = vk::AccessFlagBits::eNone,
      .dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite};

  return device.createRenderPassUnique(
      vk::RenderPassCreateInfo{.attachmentCount = static_cast<std::uint32_t>(attachment_descriptions.size()),
                               .pAttachments = attachment_descriptions.data(),
                               .subpassCount = 1,
                               .pSubpasses = &kSubpassDescription,
                               .dependencyCount = 1,
                               .pDependencies = &kSubpassDependency});
}

std::vector<vk::UniqueFramebuffer> CreateFramebuffers(const vk::Device device,
                                                      const gfx::Swapchain& swapchain,
                                                      const vk::RenderPass render_pass,
                                                      const vk::ImageView color_attachment,
                                                      const vk::ImageView depth_attachment) {
  return swapchain.image_views()
         | std::views::transform([=, extent = swapchain.image_extent()](const auto color_resolve_attachment) {
             const std::array image_attachments{color_attachment, color_resolve_attachment, depth_attachment};
             return device.createFramebufferUnique(
                 vk::FramebufferCreateInfo{.renderPass = render_pass,
                                           .attachmentCount = static_cast<std::uint32_t>(image_attachments.size()),
                                           .pAttachments = image_attachments.data(),
                                           .width = extent.width,
                                           .height = extent.height,
                                           .layers = 1});
           })
         | std::ranges::to<std::vector>();
}

vk::UniquePipelineLayout CreateGraphicsPipelineLayout(const vk::Device device) {
  static constexpr vk::PushConstantRange kPushConstantRange{.stageFlags = vk::ShaderStageFlagBits::eVertex,
                                                            .offset = 0,
                                                            .size = sizeof(VertexTransforms)};

  return device.createPipelineLayoutUnique(
      vk::PipelineLayoutCreateInfo{.pushConstantRangeCount = 1, .pPushConstantRanges = &kPushConstantRange});
}

vk::UniquePipeline CreateGraphicsPipeline(const vk::Device device,
                                          const vk::Extent2D swapchain_image_extent,
                                          const vk::SampleCountFlagBits msaa_sample_count,
                                          const vk::PipelineLayout pipeline_layout,
                                          const vk::RenderPass render_pass) {
  const std::filesystem::path vertex_shader_filepath{"assets/shaders/mesh.vert"};
  const gfx::ShaderModule vertex_shader_module{device, vk::ShaderStageFlagBits::eVertex, vertex_shader_filepath};

  const std::filesystem::path fragment_shader_filepath{"assets/shaders/mesh.frag"};
  const gfx::ShaderModule fragment_shader_module{device, vk::ShaderStageFlagBits::eFragment, fragment_shader_filepath};

  const std::array shader_stage_create_info{
      vk::PipelineShaderStageCreateInfo{.stage = vk::ShaderStageFlagBits::eVertex,
                                        .module = *vertex_shader_module,
                                        .pName = "main"},
      vk::PipelineShaderStageCreateInfo{.stage = vk::ShaderStageFlagBits::eFragment,
                                        .module = *fragment_shader_module,
                                        .pName = "main"}};

  static constexpr vk::VertexInputBindingDescription kVertexInputBindingDescription{
      .binding = 0,
      .stride = sizeof(gfx::Mesh::Vertex),
      .inputRate = vk::VertexInputRate::eVertex};

  static constexpr std::array kVertexAttributeDescriptions{
      vk::VertexInputAttributeDescription{.location = 0,
                                          .binding = 0,
                                          .format = vk::Format::eR32G32B32Sfloat,
                                          .offset = offsetof(gfx::Mesh::Vertex, position)},
      vk::VertexInputAttributeDescription{.location = 1,
                                          .binding = 0,
                                          .format = vk::Format::eR32G32Sfloat,
                                          .offset = offsetof(gfx::Mesh::Vertex, texture_coordinates)},
      vk::VertexInputAttributeDescription{.location = 2,
                                          .binding = 0,
                                          .format = vk::Format::eR32G32B32Sfloat,
                                          .offset = offsetof(gfx::Mesh::Vertex, normal)}};

  static constexpr vk::PipelineVertexInputStateCreateInfo kVertexInputStateCreateInfo{
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &kVertexInputBindingDescription,
      .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(kVertexAttributeDescriptions.size()),
      .pVertexAttributeDescriptions = kVertexAttributeDescriptions.data()};

  static constexpr vk::PipelineInputAssemblyStateCreateInfo kInputAssemblyStateCreateInfo{
      .topology = vk::PrimitiveTopology::eTriangleList};

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

  static constexpr vk::PipelineDepthStencilStateCreateInfo kDepthStencilStateCreateInfo{
      .depthTestEnable = vk::True,
      .depthWriteEnable = vk::True,
      .depthCompareOp = vk::CompareOp::eLess};

  const vk::PipelineMultisampleStateCreateInfo multisample_state_create_info{.rasterizationSamples = msaa_sample_count};

  using enum vk::ColorComponentFlagBits;
  static constexpr vk::PipelineColorBlendAttachmentState kColorBlendAttachmentState{
      .blendEnable = vk::True,
      .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
      .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
      .colorBlendOp = vk::BlendOp::eAdd,
      .srcAlphaBlendFactor = vk::BlendFactor::eOne,
      .dstAlphaBlendFactor = vk::BlendFactor::eZero,
      .alphaBlendOp = vk::BlendOp::eAdd,
      .colorWriteMask = eR | eG | eB | eA};

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
                                     .pMultisampleState = &multisample_state_create_info,
                                     .pDepthStencilState = &kDepthStencilStateCreateInfo,
                                     .pColorBlendState = &kColorBlendStateCreateInfo,
                                     .layout = pipeline_layout,
                                     .renderPass = render_pass,
                                     .subpass = 0});
  vk::resultCheck(result, "Graphics pipeline creation failed");

  return std::move(graphics_pipeline);  // return value optimization not available here
}

vk::UniqueCommandPool CreateCommandPool(const gfx::Device& device) {
  return device->createCommandPoolUnique(
      vk::CommandPoolCreateInfo{.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                .queueFamilyIndex = device.physical_device().queue_family_indices().graphics_index});
}

template <std::size_t N>
std::vector<vk::UniqueCommandBuffer> AllocateCommandBuffers(const vk::Device device,
                                                            const vk::CommandPool command_pool) {
  return device.allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo{.commandPool = command_pool,
                                                                           .level = vk::CommandBufferLevel::ePrimary,
                                                                           .commandBufferCount = N});
}

template <std::size_t N>
std::array<vk::UniqueSemaphore, N> CreateSemaphores(const vk::Device device) {
  std::array<vk::UniqueSemaphore, N> semaphores;
  std::ranges::generate(semaphores, [device] {  // NOLINT(whitespace/newline)
    return device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
  });
  return semaphores;
}

template <std::size_t N>
std::array<vk::UniqueFence, N> CreateFences(const vk::Device device) {
  std::array<vk::UniqueFence, N> fences;
  std::ranges::generate(fences, [device] {
    return device.createFenceUnique(vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
  });
  return fences;
}

}  // namespace

namespace gfx {

Engine::Engine(const Window& window)
    : surface_{window.CreateSurface(*instance_)},
      device_{*instance_, *surface_},
      swapchain_{window, *surface_, device_},
      msaa_sample_count_{GetMsaaSampleCount(device_.physical_device().limits())},
      color_attachment_{device_,
                        swapchain_.image_format(),
                        swapchain_.image_extent(),
                        msaa_sample_count_,
                        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
                        vk::ImageAspectFlagBits::eColor,
                        vk::MemoryPropertyFlagBits::eDeviceLocal},
      depth_attachment_{device_,
                        GetDepthAttachmentFormat(*device_.physical_device()),
                        swapchain_.image_extent(),
                        msaa_sample_count_,
                        vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
                        vk::ImageAspectFlagBits::eDepth,
                        vk::MemoryPropertyFlagBits::eDeviceLocal},
      render_pass_{
          CreateRenderPass(*device_, msaa_sample_count_, swapchain_.image_format(), depth_attachment_.format())},
      framebuffers_{CreateFramebuffers(*device_,
                                       swapchain_,
                                       *render_pass_,
                                       color_attachment_.image_view(),
                                       depth_attachment_.image_view())},
      graphics_pipeline_layout_{CreateGraphicsPipelineLayout(*device_)},
      graphics_pipeline_{CreateGraphicsPipeline(*device_,
                                                swapchain_.image_extent(),
                                                msaa_sample_count_,
                                                *graphics_pipeline_layout_,
                                                *render_pass_)},
      command_pool_{CreateCommandPool(device_)},
      command_buffers_{AllocateCommandBuffers<kMaxRenderFrames>(*device_, *command_pool_)},
      acquire_next_image_semaphores_{CreateSemaphores<kMaxRenderFrames>(*device_)},
      present_image_semaphores_{CreateSemaphores<kMaxRenderFrames>(*device_)},
      draw_fences_{CreateFences<kMaxRenderFrames>(*device_)} {}

void Engine::Render(const ArcCamera& camera, const Mesh& mesh) {
  if (++current_frame_index_ == kMaxRenderFrames) {
    current_frame_index_ = 0;
  }
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-constant-array-index)
  const auto draw_fence = *draw_fences_[current_frame_index_];
  const auto acquire_next_image_semaphore = *acquire_next_image_semaphores_[current_frame_index_];
  const auto present_image_semaphore = *present_image_semaphores_[current_frame_index_];
  // NOLINTEND(cppcoreguidelines-pro-bounds-constant-array-index)

  static constexpr auto kMaxTimeout = std::numeric_limits<std::uint64_t>::max();
  auto result = device_->waitForFences(draw_fence, vk::True, kMaxTimeout);
  vk::resultCheck(result, "Draw fence failed to enter a signaled state");
  device_->resetFences(draw_fence);

  std::uint32_t image_index = 0;
  std::tie(result, image_index) = device_->acquireNextImageKHR(*swapchain_, kMaxTimeout, acquire_next_image_semaphore);
  vk::resultCheck(result, "Failed to acquire the next presentable image");

  const auto command_buffer = *command_buffers_[current_frame_index_];
  command_buffer.begin(vk::CommandBufferBeginInfo{.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
  command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphics_pipeline_);

  static constexpr std::array kClearColor{0.05098039f, 0.06666667f, 0.08627451f, 1.0f};
  static constexpr std::array kClearValues{vk::ClearValue{.color = vk::ClearColorValue{kClearColor}},
                                           vk::ClearValue{.color = vk::ClearColorValue{kClearColor}},
                                           vk::ClearValue{.depthStencil = vk::ClearDepthStencilValue{1.0f, 0}}};
  command_buffer.beginRenderPass(
      vk::RenderPassBeginInfo{
          .renderPass = *render_pass_,
          .framebuffer = *framebuffers_[image_index],
          .renderArea = vk::Rect2D{.offset = vk::Offset2D{0, 0}, .extent = swapchain_.image_extent()},
          .clearValueCount = static_cast<std::uint32_t>(kClearValues.size()),
          .pClearValues = kClearValues.data()},
      vk::SubpassContents::eInline);

  command_buffer.pushConstants<VertexTransforms>(
      *graphics_pipeline_layout_,
      vk::ShaderStageFlagBits::eVertex,
      0,
      VertexTransforms{.model_view_transform = camera.GetViewTransform() * mesh.transform(),
                       .projection_transform = camera.GetProjectionTransform()});

  mesh.Render(command_buffer);

  command_buffer.endRenderPass();
  command_buffer.end();

  static constexpr vk::PipelineStageFlags kPipelineWaitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  device_.graphics_queue().submit(vk::SubmitInfo{.waitSemaphoreCount = 1,
                                                 .pWaitSemaphores = &acquire_next_image_semaphore,
                                                 .pWaitDstStageMask = &kPipelineWaitStage,
                                                 .commandBufferCount = 1,
                                                 .pCommandBuffers = &command_buffer,
                                                 .signalSemaphoreCount = 1,
                                                 .pSignalSemaphores = &present_image_semaphore},
                                  draw_fence);

  const auto swapchain = *swapchain_;
  result = device_.present_queue().presentKHR(vk::PresentInfoKHR{.waitSemaphoreCount = 1,
                                                                 .pWaitSemaphores = &present_image_semaphore,
                                                                 .swapchainCount = 1,
                                                                 .pSwapchains = &swapchain,
                                                                 .pImageIndices = &image_index});
  vk::resultCheck(result, "Failed to queue an image for presentation");
}

}  // namespace gfx
