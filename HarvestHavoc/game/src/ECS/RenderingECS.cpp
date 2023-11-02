/// \file    RenderingSystems.cpp
/// \author  Matthew Green
/// \date    10/31/2023 12:32:19
/// 
/// \section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "ECS/RenderingECS.h"

#include <Engine/vk_engine.h>
#include <Engine/AllocatedBuffer.h>
#include <Engine/vk_initializers.h>
#include <Engine/vk_shaders.h>
#include <Engine/vk_pipeline.h>

#include <Graphics/MeshPushConstants.h>

#include "Engine/vk_initializers.h"
#include "Engine/vk_pipeline.h"
#include "Engine/vk_shaders.h"

#include "Graphics/Color32.h"

#include "FileManagement/Path.h"

#include <iostream>
#include <fstream>
#include <chrono>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <VkBootstrap/VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_vulkan.h>

#define VK_CHECK(x)                                                    \
    do                                                                 \
    {                                                                  \
        VkResult err = x;                                              \
        if (err)                                                       \
        {                                                              \
            std::cout <<"Detected Vulkan error: " << err << std::endl; \
            abort();                                                   \
        }                                                              \
    } while (0)

using namespace engine;

namespace hh {

// Public Fields

// Constructors and Destructors

RenderingECS::RenderingECS(flecs::world& ecs, ECSPipelineStages& stages, VulkanEngine& engine)
    : IRenderingECS(ecs, engine), stages(stages) {}

// Public Methods

void RenderingECS::Init()
{
    InitVulkan();
    InitSwapchain();
    InitCommands();
    InitDefaultRenderPass();
    InitFrameBuffers();
    InitSyncStructures();
    InitPipelines();

    InitImGUI();

    LoadMeshes();

    IRenderingECS::Init();
}

void RenderingECS::Cleanup()
{
    //make sure the GPU has stopped doing its things
    vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);

    _mainDeletionQueue.Flush();

    vmaDestroyAllocator(_allocator);
    vkDestroyDevice(_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
    vkDestroyInstance(_instance, nullptr);
    SDL_DestroyWindow(engine._window);
}

// Protected Fields

// Protected Methods

void RenderingECS::InitComponents()
{
    ecs.component<Transform>();
    ecs.component<Mesh>();
    ecs.component<Material>();
}

void RenderingECS::InitSystems()
{
    //ecs.system<Transform, Mesh, Material>()
    //    .kind<Draw>()
    //    .iter([](flecs::iter& it, Transform* transforms, Mesh* meshes, Material* materials)
    //    {
    //        float delta_time = it.delta_time();

    //        // Vulkan Start Code

    //        vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    //        for (auto i : it)
    //        {
    //            Transform transform = transforms[i];
    //            Mesh mesh = meshes[i];
    //            Material material = materials[i];

    //            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material._meshPipeline);

    //            //bind the mesh vertex buffer with offset 0
    //            VkDeviceSize offset = 0;
    //            vkCmdBindVertexBuffers(cmd, 0, 1, &mesh._vertexBuffer._buffer, &offset);

    //            MeshPushConstants constants = {};
    //            constants.renderMatrix = mesh.GetRenderMatrix(material);

    //            //upload the matrix to the GPU via push constants
    //            vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

    //            //we can now draw the mesh
    //            vkCmdDraw(cmd, (uint32_t)_triangleMesh._vertices.size(), 1, 0, 0);
    //        }

    //        vkCmdEndRenderPass(cmd);

    //        // Vulkan End Code
    //    }
    //);

    ecs.system()
        .kind(stages.PreDraw)
        .iter([this](flecs::iter& it)
        {
            float deltaTime = it.delta_time();

            //wait until the GPU has finished rendering the last frame. Timeout of 1 second
            VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000));
            VK_CHECK(vkResetFences(_device, 1, &_renderFence));

            //request image from the swapchain, one second timeout
            VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, _presentSemaphore, nullptr, &swapchainImageIndex));

            //now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
            VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer, 0));

            //begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
            VkCommandBufferBeginInfo cmdBeginInfo = {};
            cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            cmdBeginInfo.pNext = nullptr;

            cmdBeginInfo.pInheritanceInfo = nullptr;
            cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            VK_CHECK(vkBeginCommandBuffer(_mainCommandBuffer, &cmdBeginInfo));

            //make a clear-color from frame number. This will flash with a 120*pi frame period.
            VkClearValue clearValue = {};
            float flash = abs(sin(engine._frameNumber / 3840.f));
            clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

            //start the main renderpass.
            //We will use the clear color from above, and the framebuffer of the index the swapchain gave us
            VkRenderPassBeginInfo rpInfo = {};
            rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpInfo.pNext = nullptr;

            rpInfo.renderPass = _renderPass;
            rpInfo.renderArea.offset.x = 0;
            rpInfo.renderArea.offset.y = 0;
            rpInfo.renderArea.extent = engine._windowExtent;
            rpInfo.framebuffer = _framebuffers[swapchainImageIndex];

            //connect clear values
            rpInfo.clearValueCount = 1;
            rpInfo.pClearValues = &clearValue;

            vkCmdBeginRenderPass(_mainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
        }
    );

    ecs.system()
        .kind(stages.PostDraw)
        .iter([this](flecs::iter& it)
        {
            float deltaTime = it.delta_time();

            //finalize the render pass
            vkCmdEndRenderPass(_mainCommandBuffer);
            //finalize the command buffer (we can no longer add commands, but it can now be executed)
            VK_CHECK(vkEndCommandBuffer(_mainCommandBuffer));


            //prepare the submission to the queue.
            //we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
            //we will signal the _renderSemaphore, to signal that rendering has finished

            VkSubmitInfo submit = {};
            submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit.pNext = nullptr;

            VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            submit.pWaitDstStageMask = &waitStage;

            submit.waitSemaphoreCount = 1;
            submit.pWaitSemaphores = &_presentSemaphore;

            submit.signalSemaphoreCount = 1;
            submit.pSignalSemaphores = &_renderSemaphore;

            submit.commandBufferCount = 1;
            submit.pCommandBuffers = &_mainCommandBuffer;

            //submit command buffer to the queue and execute it.
            // _renderFence will now block until the graphic commands finish execution
            VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));


            // this will put the image we just rendered into the visible window.
            // we want to wait on the _renderSemaphore for that,
            // as it's necessary that drawing commands have finished before the image is displayed to the user
            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.pNext = nullptr;

            presentInfo.pSwapchains = &_swapchain;
            presentInfo.swapchainCount = 1;

            presentInfo.pWaitSemaphores = &_renderSemaphore;
            presentInfo.waitSemaphoreCount = 1;

            presentInfo.pImageIndices = &swapchainImageIndex;

            VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

            //increase the number of frames drawn
            engine._frameNumber++;
        }
    );

    ecs.system<Transform, Mesh, Material>()
        .kind(stages.Draw)
        .iter([this](flecs::iter& it, Transform* transforms, Mesh* meshes, Material* materials)
        {
            float deltaTime = it.delta_time();

            for (auto i : it)
            {
                Transform transform = transforms[i];
                Mesh mesh = meshes[i];
                Material material = materials[i];

                const auto* globalData = it.world().get<GlobalData>();
                flecs::entity camera(it.world(), globalData->camera);

                vkCmdBindPipeline(_mainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

                //bind the mesh vertex buffer with offset 0
                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(_mainCommandBuffer, 0, 1, &mesh._vertexBuffer._buffer, &offset);

                MeshPushConstants constants = {};
                constants.renderMatrix = GetRenderMatrix(transform, camera);

                //upload the matrix to the GPU via push constants
                vkCmdPushConstants(_mainCommandBuffer, material.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

                //we can now draw the mesh
                vkCmdDraw(_mainCommandBuffer, (uint32_t)mesh._vertices.size(), 1, 0, 0);
            }
        }
    );
}

void RenderingECS::InitEntities()
{
    OrthoCamera ortho{Rect{Vec2::zero(), engine::Vec2{1700.0f, 900.0f}}};
    auto camera = ecs.entity("Camera")
        .set<Transform>({})
        .set<OrthoCamera>(ortho);
    
    ecs.set<GlobalData>({CreatePerspectiveCamera(Vec3{0.0f, 0.0f,-2.0f}, Vec3::ZERO, Vec2{1700.0f, 900.0f})});

    ecs.entity()
        .set<Transform>({Vec3::ZERO, Vec3{-180.0f, 0.0f, 0.0f}, Vec3::ONE * 0.5f})
        .set<Mesh>({ _triangleMesh._vertices, _triangleMesh._vertexBuffer })
        .set<Material>({engine::Color32::MAGENTA, _meshPipeline, _meshPipelineLayout})
        .set<LinearKinematics>({Vec3{0.0f, 0.00f, 0.00f}, Vec3::ZERO})
        .set<AngularKinematics>({Vec3{0.0f, 0.0f, -90.0f}, Vec3::ZERO});
}

// Private Fields

// Private Methods

void RenderingECS::InitVulkan()
{
    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("HarvestHavoc")
    #ifdef _DEBUG
        .request_validation_layers(true)
    #endif
        .require_api_version(1, 1, 0)
        .use_default_debug_messenger()
        .build();
    
    vkb::Instance vkb_inst = inst_ret.value();

    //store the instance
    _instance = vkb_inst.instance;
    //store the debug messenger
    _debug_messenger = vkb_inst.debug_messenger;

    // get the surface of the window we opened with SDL
    SDL_Vulkan_CreateSurface(engine._window, _instance, &_surface);

    //use vkbootstrap to select a GPU.
    //We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{ vkb_inst };

    // Create a VkPhysicalDeviceFeatures structure and set the fillModeNonSolid feature to VK_TRUE
    VkPhysicalDeviceFeatures desiredFeatures = {};
    desiredFeatures.fillModeNonSolid = VK_TRUE;

    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 1)
        .set_surface(_surface)
        .set_required_features(desiredFeatures)
        .select()
        .value();

    //create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };
    // automatically propagate needed data from instance & physical device
    auto dev_ret = deviceBuilder.build();
    if (!dev_ret)
    {
        std::cerr << "Failed to create Vulkan device. Error: " << dev_ret.error().message() << "\n";
        exit(EXIT_FAILURE);
    }

    vkb::Device vkbDevice = dev_ret.value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    _device = vkbDevice.device;
    _chosenGPU = physicalDevice.physical_device;

    // use vkbootstrap to get a Graphics queue
    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    //initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosenGPU;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);
}

void RenderingECS::InitSwapchain()
{
    vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU,_device,_surface };

    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .use_default_format_selection()
        //use vsync present mode
        // .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        // .set_desired_extent(_windowExtent.width, _windowExtent.height)
        .build()
        .value();
    
    vkbSwapchain.extent = engine._windowExtent;
    vkbSwapchain.present_mode = VK_PRESENT_MODE_FIFO_KHR;

    //store swapchain and its related images
    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();

    _swapchainImageFormat = vkbSwapchain.image_format;

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        }
    );
}

void RenderingECS::InitCommands()
{
    //create a command pool for commands submitted to the graphics queue.
    //we also want the pool to allow for resetting of individual command buffers
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

    //allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);

    VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroyCommandPool(_device, _commandPool, nullptr);
        }
    );
}

void RenderingECS::InitDefaultRenderPass()
{
    // the renderpass will use this color attachment.
    VkAttachmentDescription color_attachment = {};
    //the attachment will have the format needed by the swapchain
    color_attachment.format = _swapchainImageFormat;
    //1 sample, we won't be doing MSAA
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // we Clear when this attachment is loaded
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // we keep the attachment stored when the renderpass ends
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    //we don't care about stencil
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    //we don't know or care about the starting layout of the attachment
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    //after the renderpass ends, the image has to be on a layout ready for display
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkAttachmentReference color_attachment_ref = {};
    //attachment number will index into the pAttachments array in the parent renderpass itself
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //we are going to create 1 subpass, which is the minimum you can do
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;


    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    //connect the color attachment to the info
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    //connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroyRenderPass(_device, _renderPass, nullptr);
        }
    );
}

void RenderingECS::InitFrameBuffers()
{
    //create the framebuffers for the swapchain images. This will connect the render-pass to the images for rendering
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = _renderPass;
    fb_info.attachmentCount = 1;
    fb_info.width = engine._windowExtent.width;
    fb_info.height = engine._windowExtent.height;
    fb_info.layers = 1;

    //grab how many images we have in the swapchain
    const size_t swapchain_imagecount = _swapchainImages.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    //create framebuffers for each of the swapchain image views
    for (int i = 0; i < swapchain_imagecount; i++)
    {
        fb_info.pAttachments = &_swapchainImageViews[i];
        VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));

        _mainDeletionQueue.PushDeletor
        (
            [=]()
            {
                vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
                vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
            }
        );
    }
}

void RenderingECS::InitSyncStructures()
{
    //we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
    VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroyFence(_device, _renderFence, nullptr);
        }
    );

    //for the semaphores we don't need any flags
    VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();

    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore));

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vkDestroySemaphore(_device, _presentSemaphore, nullptr);
            vkDestroySemaphore(_device, _renderSemaphore, nullptr);
        }
    );
}

void RenderingECS::InitPipelines()
{
    //build the pipeline layout that controls the inputs/outputs of the shader
    //we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
    VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();

    VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_trianglePipelineLayout));


    //build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage
    PipelineBuilder pipelineBuilder;

    auto triangleVertShader = ShaderModule::CreateVertexShaderModule(_device, "ColoredTriangle.vert.spv");
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, *triangleVertShader));
    auto triangleFragShader = ShaderModule::CreateFragmentShaderModule(_device, "ColoredTriangle.frag.spv");
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, *triangleFragShader));


    //vertex input controls how to read vertices from vertex buffers. We aren't using it yet
    pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

    //input assembly is the configuration for drawing triangle lists, strips, or individual points.
    //we are just going to draw triangle list
    pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    //build viewport and scissor from the swapchain extents
    pipelineBuilder._viewport.x = 0.0f;
    pipelineBuilder._viewport.y = 0.0f;
    pipelineBuilder._viewport.width = (float)engine._windowExtent.width;
    pipelineBuilder._viewport.height = (float)engine._windowExtent.height;
    pipelineBuilder._viewport.minDepth = 0.0f;
    pipelineBuilder._viewport.maxDepth = 1.0f;

    pipelineBuilder._scissor.offset = { 0, 0 };
    pipelineBuilder._scissor.extent = engine._windowExtent;

    //configure the rasterizer to draw filled triangles
    pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

    //we don't use multisampling, so just run the default one
    pipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

    //a single blend attachment with no blending and writing to RGBA
    pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

    //use the triangle layout we created
    pipelineBuilder._pipelineLayout = _trianglePipelineLayout;

    //finally build the pipeline
    _trianglePipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    //clear the shader stages for the builder
    pipelineBuilder._shaderStages.clear();

    //add the other shaders
    auto redTriangleVertShader = ShaderModule::CreateVertexShaderModule(_device, "triangle.vert.spv");
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, *redTriangleVertShader));
    auto redTriangleFragShader = ShaderModule::CreateFragmentShaderModule(_device, "triangle.frag.spv");
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, *redTriangleFragShader));

    //build the red triangle pipeline
    _redTrianglePipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_LINE);

    // Build the wireframe pipeline
    _triangleWireFramePipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

    //clear the shader stages for the builder
    pipelineBuilder._shaderStages.clear();

    //add the other shaders
    auto rainbowTriangleVertShader = ShaderModule::CreateVertexShaderModule(_device, "RainbowTriangle.vert.spv");
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, *rainbowTriangleVertShader));
    auto rainbowTriangleFragShader = ShaderModule::CreateFragmentShaderModule(_device, "RainbowTriangle.frag.spv");
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, *rainbowTriangleFragShader));

    _rainbowTrianglePipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    //we start from just the default empty pipeline layout info
	VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();

	//setup push constants
    VkPushConstantRange push_constant = {};
	//this push constant range starts at the beginning
	push_constant.offset = 0;
	//this push constant range takes up the size of a MeshPushConstants struct
	push_constant.size = sizeof(MeshPushConstants);
	//this push constant range is accessible only in the vertex shader
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	mesh_pipeline_layout_info.pPushConstantRanges = &push_constant;
	mesh_pipeline_layout_info.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &_meshPipelineLayout));

    //build the mesh pipeline

    VertexInputAttributeDescriptor vertexDescription = Vertex::GetVertexDescription();

    //connect the pipeline builder vertex input info to the one we get from Vertex
    pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
    pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)vertexDescription.attributes.size();

    pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
    pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)vertexDescription.bindings.size();

    //clear the shader stages for the builder
    pipelineBuilder._shaderStages.clear();

    //add the other shaders
    const auto meshVertShader = ShaderModule::CreateVertexShaderModule(_device, "TriangleMesh.vert.spv");
    pipelineBuilder._shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, *meshVertShader));

    //make sure that triangleFragShader is holding the compiled colored_triangle.frag
    const auto meshFragShader = ShaderModule::CreateFragmentShaderModule(_device, "TriangleMesh.frag.spv");
    pipelineBuilder._shaderStages.push_back(
        vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, *meshFragShader));

    pipelineBuilder._pipelineLayout = _meshPipelineLayout;

    //build the mesh triangle pipeline
    _meshPipeline = pipelineBuilder.BuildPipeline(_device, _renderPass);

    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            //destroy the pipelines we have created
            vkDestroyPipeline(_device, _redTrianglePipeline, nullptr);
            vkDestroyPipeline(_device, _trianglePipeline, nullptr);
            vkDestroyPipeline(_device, _triangleWireFramePipeline, nullptr);
            vkDestroyPipeline(_device, _rainbowTrianglePipeline, nullptr);
            vkDestroyPipeline(_device, _meshPipeline, nullptr);

            //destroy the pipeline layout that they use
            vkDestroyPipelineLayout(_device, _trianglePipelineLayout, nullptr);
            vkDestroyPipelineLayout(_device, _meshPipelineLayout, nullptr);
        }
    );
}

void RenderingECS::InitImGUI()
{
    // //1: create descriptor pool for IMGUI
    // // the size of the pool is very oversize, but it's copied from imgui demo itself.
    // VkDescriptorPoolSize pool_sizes[] =
    // {
    //     { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    //     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    //     { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    //     { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    // };

    // VkDescriptorPoolCreateInfo pool_info = {};
    // pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    // pool_info.maxSets = 1000;
    // pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    // pool_info.pPoolSizes = pool_sizes;

    // VkDescriptorPool imguiPool;
    // VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));


    // // 2: initialize imgui library

    // //this initializes the core structures of imgui
    // ImGui::CreateContext();

    // //this initializes imgui for SDL
    // ImGui_ImplSDL2_InitForVulkan(_window);

    // //this initializes imgui for Vulkan
    // ImGui_ImplVulkan_InitInfo init_info = {};
    // init_info.Instance = _instance;
    // init_info.PhysicalDevice = _chosenGPU;
    // init_info.Device = _device;
    // init_info.Queue = _graphicsQueue;
    // init_info.DescriptorPool = imguiPool;
    // init_info.MinImageCount = 3;
    // init_info.ImageCount = 3;
    // init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    // ImGui_ImplVulkan_Init(&init_info, _renderPass);

    // //execute a gpu command to upload imgui font textures
    // immediate_submit
    // (
    //     [&](VkCommandBuffer cmd)
    //     {
    //         ImGui_ImplVulkan_CreateFontsTexture(cmd);
    //     }
    // );

    // //clear font textures from cpu data
    // ImGui_ImplVulkan_DestroyFontUploadObjects();

    // //add the destroy the imgui created structures
    // _mainDeletionQueue.PushDeletor
    // (
    //     [=]()
    //     {
    //         vkDestroyDescriptorPool(_device, imguiPool, nullptr);
    //         ImGui_ImplVulkan_Shutdown();
    //     }
    // );
}

void RenderingECS::LoadMeshes()
{
    //make the array 3 vertices long
    _triangleMesh._vertices.resize(3);

    //vertex positions
    _triangleMesh._vertices[0].position = {  1.0f,  1.0f,  0.0f };
    _triangleMesh._vertices[1].position = { -1.0f,  1.0f,  0.0f };
    _triangleMesh._vertices[2].position = {  0.0f, -1.0f,  0.0f };

    //vertex colors, all green
    _triangleMesh._vertices[0].color = Color32::GREEN;
    _triangleMesh._vertices[1].color = Color32::GREEN;
    _triangleMesh._vertices[2].color = Color32::GREEN;

    //we don't care about the vertex normals

    UploadMesh(_triangleMesh);
}

void RenderingECS::UploadMesh(Mesh& mesh)
{
    //allocate vertex buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //this is the total size, in bytes, of the buffer we are allocating
    bufferInfo.size = mesh._vertices.size() * sizeof(Vertex);
    //this buffer is going to be used as a Vertex Buffer
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


    //let the VMA library know that this data should be writeable by CPU, but also readable by GPU
    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    //allocate the buffer
    VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo,
        &mesh._vertexBuffer._buffer,
        &mesh._vertexBuffer._allocation,
        nullptr));

    //add the destruction of triangle mesh buffer to the deletion queue
    _mainDeletionQueue.PushDeletor
    (
        [=]()
        {
            vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
        }
    );

    //copy vertex data
    void* data;
    vmaMapMemory(_allocator, mesh._vertexBuffer._allocation, &data);

    memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));

    vmaUnmapMemory(_allocator, mesh._vertexBuffer._allocation);
}

flecs::entity RenderingECS::CreatePerspectiveCamera(const Vec3 position /*= Vec3::ZERO*/,
    const Vec3 rotation /*= Vec3::ZERO*/,
    const Vec2 resolution /*= Vec2{1920.0f, 1080.0f}*/,
    const float verticalFOV /*= 70.0f*/,
    const float aspectRatio /*= 16.0f/9.0f*/,
    const float nearPlaneOffset /*= 0.1f*/,
    const float farPlaneOffset /*= 200.0f*/)
{
    Transform transform{position, rotation};
    Rect extent{Vec2::zero(), resolution};
    PerspectiveCamera perspective{extent, verticalFOV, aspectRatio, nearPlaneOffset, farPlaneOffset};
    return ecs.entity("Camera")
        .set<Transform>(transform)
        .set<PerspectiveCamera>(perspective);
}

glm::mat4 RenderingECS::GetRenderMatrix(const Transform& transform, const flecs::entity camera)
{
    auto cameraTransform = camera.get<Transform>();


    // Start with the identity matrix
    glm::mat4 model = glm::mat4(1.0f); 

    // Apply scaling
    model = glm::scale(model, (glm::vec3)transform.scale);

    // Apply rotation around the x, y, and z axes
    model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

    // Compute the view matrix
    glm::mat4 view = glm::translate(glm::mat4(1.f), (glm::vec3)cameraTransform->position);

    // Compute the projection matrix
    glm::mat4 projection;
    auto perspectiveCamera = camera.get<PerspectiveCamera>();
    if (perspectiveCamera)
    {
        projection = glm::perspective(glm::radians(70.f), engine._windowExtent.width / (float)engine._windowExtent.height, 0.1f, 200.0f);
    }
    else
    {
        auto orthoCamera = camera.get<OrthoCamera>();
        auto extent = orthoCamera->extent;
        glm::ortho(extent.min.x, extent.max.x, extent.min.y, extent.max.y, orthoCamera->nearPlaneOffset, orthoCamera->farPlaneOffset);
    }
    

    // Apply translation in world space
    glm::mat4 worldTranslation = glm::translate(glm::mat4(1.0f), (glm::vec3)transform.position);

    // Calculate the final mesh matrix
    glm::mat4 meshMatrix = projection * view * worldTranslation * model;

    return meshMatrix;
}

} // namespace hh
