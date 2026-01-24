// -------------------------------------------------
//
//  MIT License
//
//  v_vulkanbackend.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/22/26.
//
// -------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <array>
#include <limits>

#include <fmt/core.h>
#include <vulkan/vulkan.hpp>

#include "../../../headers/v_types.hpp"

typedef unsigned char       VkUint8;    /* 8-bit unsigned integer used for Vulkan */
typedef unsigned short      VkUint16;   /* 16-bit unsigned integer used for Vulkan*/
typedef unsigned int        VkUint32;   /* 32-bit unsigned integer used for Vulkan */
typedef unsigned long long  VkUint64;   /* 64-bit unsigned integer used for Vulkan */
typedef unsigned long long  VkUsize;   

#define VK_ERROR_STRING(x) case static_cast<int>(x): return #x;

constexpr inline const char* Vk_ErrorToString(VkResult result)
{
    switch(result)
    {
        VK_ERROR_STRING(VK_SUCCESS);
        VK_ERROR_STRING(VK_NOT_READY);
        VK_ERROR_STRING(VK_TIMEOUT);
        VK_ERROR_STRING(VK_EVENT_SET);
        VK_ERROR_STRING(VK_EVENT_RESET);
        VK_ERROR_STRING(VK_INCOMPLETE);
        VK_ERROR_STRING(VK_ERROR_OUT_OF_HOST_MEMORY);
        VK_ERROR_STRING(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        VK_ERROR_STRING(VK_ERROR_INITIALIZATION_FAILED);
        VK_ERROR_STRING(VK_ERROR_DEVICE_LOST);
        VK_ERROR_STRING(VK_ERROR_MEMORY_MAP_FAILED);
        VK_ERROR_STRING(VK_ERROR_LAYER_NOT_PRESENT);
        VK_ERROR_STRING(VK_ERROR_EXTENSION_NOT_PRESENT);
        VK_ERROR_STRING(VK_ERROR_FEATURE_NOT_PRESENT);
        VK_ERROR_STRING(VK_ERROR_INCOMPATIBLE_DRIVER);
        VK_ERROR_STRING(VK_ERROR_TOO_MANY_OBJECTS);
        VK_ERROR_STRING(VK_ERROR_FORMAT_NOT_SUPPORTED);
        VK_ERROR_STRING(VK_ERROR_SURFACE_LOST_KHR);
        VK_ERROR_STRING(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        VK_ERROR_STRING(VK_SUBOPTIMAL_KHR);
        VK_ERROR_STRING(VK_ERROR_OUT_OF_DATE_KHR);
        VK_ERROR_STRING(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        VK_ERROR_STRING(VK_ERROR_VALIDATION_FAILED_EXT);
        VK_ERROR_STRING(VK_ERROR_INVALID_SHADER_NV);
        default:
            return "UNKNOWN";
    }
}

#define VK_CHECK(x)                                             \
{                                                               \
    VKResult ret = x;                                           \
    if (ret != VK_SUCCESS)                                      \
    {                                                           \
        fmt::print("VK: {} - {}", Vk_ErrorToString(ret), #x);   \
    }                                                           \
}                                                               \

#define VK_VALIDATE(x, msg)                                     \
{                                                               \
    if (!(x))                                                   \
    {                                                           \
        fmt::print("VK: {} - {}", msg, #x);                      \
    }                                                           \
}                                                               \

using std::string, std::vector, std::ifstream, std::ios, std::set, std::numeric_limits, std::max, std::min, std::array;

namespace ValhallaEngine::Vulkan
{
    enum VVulkanMemoryUsage : VkUint8 
    {
        VMU_GPU_ONLY    = 1,
        VMU_CPU_ONLY    = 2,
        VMU_CPU_TO_GPU  = 3,
        VMU_GPU_TO_CPU  = 4,
        VMU_UNDEFINED   = 0
    };

    enum VVulkanAllocationType : VkUint8 
    {
        VAT_FREE            = 1,
        VAT_BUFFER          = 2,
        VAT_IMAGE           = 3,
        VAT_IMAGE_LINEAR    = 4,
        VAT_IMAGE_OPTIMAL   = 5,
        VAT_UNDEFINED       = 0
    };

    struct VVulkanAllocation 
    {
        VkUint32 id;
        VkDevice DeviceMemory;
        VkDeviceSize offset;
        VkDeviceSize size;
        ubyte* data;
        VVulkanAllocation() :
            id(0),
            DeviceMemory(VK_NULL_HANDLE),
            offset(0),
            size(0),
            data(nullptr) {

            }
    };

    struct VVulkanChunk 
    {
        VkUint32                id;
        VkDeviceSize            size;
        VkDeviceSize            offset;
        VVulkanChunk*           next;
        VVulkanChunk*           previous;
        VVulkanAllocationType   type;
        VVulkanMemoryUsage      usage;
        VVulkanChunk() :
            id(0),
            size(0),
            offset(0),
            next(nullptr),
            previous(nullptr),
            type(VAT_UNDEFINED),
            usage(VMU_UNDEFINED) {

            }
    };

    struct VVulkanQueueFamilyIndices
    {
        VkUint32 GraphicsFamily;
        VkUint32 PresentFamily;
        bool DoesGraphicsHasValue      = false;
        bool DoesPresentFamilyHasValue = false;
        bool IsComplete() 
        {
            return DoesGraphicsHasValue && DoesPresentFamilyHasValue;
        }
    }; 

    struct VVulkanSwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR    capabilities;
        vector<VkSurfaceFormatKHR>  formats;
        vector<VkPresentModeKHR>    PresentModes;
    };

    struct VVulkanPipelineConfigInfo 
    {
        VkViewport                              viewport;
        VkRect2D                                scissor;
        VkPipelineViewportStateCreateInfo       ViewportInfo;
        VkPipelineInputAssemblyStateCreateInfo  InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo  RasterizationInfo;
        VkPipelineMultisampleStateCreateInfo    MultisampleInfo;
        VkPipelineColorBlendAttachmentState     ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo     ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo   DepthStencilInfo;
        VkPipelineLayout                        PipelineLayout = nullptr;
        VkRenderPass                            RenderPass = nullptr;
        VkUint32                                Subpass = 0;
    };

    class VVulkanPipeline
    {
    public:
        /**
         * @brief Constructor for the Vulkan Pipeline for the Valhalla Engine backend
         * @param VertexFilepath The filepath to the compiled Vulkan vertex shader SPIR-V file
         * @param FragmentFilepath The filepath to the compiled Vulkan fragment shader SPIR-V file
         */
        VVulkanPipeline(const string& VertexFilepath, const string& FragmentFilepath);

        ~VVulkanPipeline();

        VVulkanPipeline(const VVulkanPipeline&) = delete;
        void operator=(const VVulkanPipeline&)  = delete;

        /**
         * @brief Creates the Vulkan shader module from the given compiled SPIR-V shader
         * @param filepath The filepath to the compiled SPIR-V shader
         * @return A vector of characters representing the shader bytecode
         */
        static vector<char> ReadShaderFile(const string& filepath);
    };

    inline VkDevice                     VDevice             = VK_NULL_HANDLE;
    inline VkPhysicalDevice             VPhysicalDevice     = VK_NULL_HANDLE;
    inline VkDeviceMemory               VDeviceMemory       = VK_NULL_HANDLE;
    inline VkDeviceSize                 VSize               = 0;
    inline VkDeviceSize                 VAllocated          = 0;
    inline VkPhysicalDeviceProperties   VPhysicalDeviceProperties;
    inline VkSurfaceKHR                 VSurface            = VK_NULL_HANDLE;
    inline VkUint32                     VMemoryTypeIndex    = (VkUint32)-1;
    inline VkUint32                     VQueueFamily        = (VkUint32)-1;
    inline VkUint32                     VNextBlockID        = (VkUint32)-1;
    inline VkUint32                     VExtensionCount     = (VkUint32)-1;
    inline VkQueue                      VQueue              = VK_NULL_HANDLE;
    inline VkPipelineCache              VPipelineCache      = VK_NULL_HANDLE;
    inline VkCommandPool                VCommandPool        = VK_NULL_HANDLE;
    inline VkDescriptorPool             VDescriptorPool     = VK_NULL_HANDLE;
    const inline vector<const char*>    VDeviceExtensions   = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    inline constexpr VkUint32           VMinImageCount      = 2;
    inline ubyte*                       VData               = nullptr;
    inline VVulkanMemoryUsage           VUsage              = VVulkanMemoryUsage::VMU_UNDEFINED;
    inline VVulkanChunk*                VHead               = nullptr;
    inline vector<const char*>          VInstanceExtensions;
    inline VkQueue                      VGraphicsQueue      = VK_NULL_HANDLE;
    inline VkQueue                      VPresentQueue       = VK_NULL_HANDLE;
    inline VkPipeline                   VGraphicsPipeline   = VK_NULL_HANDLE;
    inline VkSwapchainKHR               VSwapchain          = VK_NULL_HANDLE;
    inline VkRenderPass                 VRenderPass         = VK_NULL_HANDLE;
    inline VkFormat                     VSwapchainImageFormat;
    inline VkExtent2D                   VSwapchainExtent;
    inline vector<VkImage>              VSwapchainImages;
    inline vector<VkFramebuffer>        VSwapchainFramebuffers;
    inline vector<VkImageView>          VSwapchainImageViews;
    inline vector<VkImage>              VDepthImages;
    inline vector<VkDeviceMemory>       VDepthImagesMemories;
    inline vector<VkImageView>          VDepthImageViews;
    inline vector<VkSemaphore>          VImageAvailableSemaphores;
    inline vector<VkSemaphore>          VRenderFinishedSemaphores;
    inline vector<VkFence>              VInFlightFences;
    inline vector<VkFence>              VImagesInFlight;
    inline VkUint32                     VCurrentFrame       = 0;
    inline constexpr int                VMaxFramesInFlights = 2;
}
