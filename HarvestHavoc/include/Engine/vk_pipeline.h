/// \file    vk_pipeline.h
/// \author  Matthew Green
/// \date    10/23/2023 13:11:05
/// 
/// \section LICENSE
/// 
/// Copyright (c) 2023 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <Engine/vk_types.h>

#include <vector>

namespace HarvestHavoc::Engine {

/// \class PipelineBuilder
/// \brief Utility class for constructing Vulkan pipelines.
///
/// This class encapsulates various settings and parameters used to construct 
/// a Vulkan pipeline, offering a more structured and potentially reusable way 
/// to manage pipeline creation.
class PipelineBuilder {
public:
    // Enums

    // Public Fields
    std::vector<VkPipelineShaderStageCreateInfo> _shaderStages; /// \brief Collection of shader stages used in the pipeline.
    VkPipelineVertexInputStateCreateInfo _vertexInputInfo; /// \brief Description of the format of the vertex data.
    VkPipelineInputAssemblyStateCreateInfo _inputAssembly; /// \brief Information about the type of geometry primitives to be processed.
    VkViewport _viewport; /// \brief The viewport transformation to be applied.
    VkRect2D _scissor; /// \brief The scissor test to be applied.
    VkPipelineRasterizationStateCreateInfo _rasterizer; /// \brief Rasterization state parameters.
    VkPipelineColorBlendAttachmentState _colorBlendAttachment; /// \brief Color blending settings for the pipeline.
    VkPipelineMultisampleStateCreateInfo _multisampling; /// \brief Multisampling state parameters.
    VkPipelineLayout _pipelineLayout; /// \brief The layout of the pipeline, describing shader stages and more.

    // Constructors and Destructors
    
    // Public Methods

    /// \brief Builds and returns a Vulkan pipeline using the specified device and render pass.
    ///
    /// \param device The Vulkan device to use for pipeline creation.
    /// \param pass The render pass with which this pipeline will be used.
    /// \return The created Vulkan pipeline.
    VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass);

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    // Private Methods
};

} // namespace HarvestHavoc::Engine
