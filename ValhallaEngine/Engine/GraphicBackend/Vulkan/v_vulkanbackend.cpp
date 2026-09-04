// -------------------------------------------------
//
//  MIT License
//
//  v_vulkanbackend.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/22/26.
//
// -------------------------------------------------

#include "headers/v_vulkanbackend.hpp"
#include "../../headers/v_platform.h"

using namespace ValhallaEngine::Vulkan;

using namespace ValhallaEngine::Platform;

namespace ValhallaEngine
{
    static bool Vulkan::IsExtensionAvailable(const vector<VkExtensionProperties>& properties, const char* extension)
    {
        for (const VkExtensionProperties& p : properties)
        {
            if (strcmp(p.extensionName, extension) == 0)
            {
                return true;
            }
        }
        
        return false;
    }

    static bool Vulkan::IsDeviceSuitable(VkPhysicalDevice device)
    {
        VVulkanQueueFamilyIndices indices = FindQueueFamilies(device);

        bool ExtensionsSupported = CheckDeviceExtensionsSupport(device);

        bool SwapchainAdequate = false;
        if (ExtensionsSupported)
        {
            VVulkanSwapChainSupportDetails SwapchainSupport = QuerySwapchainSupport(device);
            SwapchainAdequate = !SwapchainSupport.formats.empty() && !SwapchainSupport.PresentModes.empty();
        }

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        return indices.IsComplete() && ExtensionsSupported && SwapchainAdequate && features.samplerAnisotropy;
    }

    static bool Vulkan::CheckDeviceExtensionsSupport(VkPhysicalDevice device)
    {
        VkUint32 ExtensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtensionCount, nullptr);

        vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &ExtensionCount, AvailableExtensions.data()));

        set<string> RequiredExtensions(VInstanceExtensions.begin(), VInstanceExtensions.end());

        for (const VkExtensionProperties& Extensions : AvailableExtensions)
        {
            RequiredExtensions.erase(Extensions.extensionName);
        }
 
        return RequiredExtensions.empty();
    }

    // Just a habit for putting void in a function with no parameters
    static bool Vulkan::InitVulkanRHI(void)
    {
        /*Check if the memory type index is valid*/
        if (VMemoryTypeIndex == numeric_limits<VkUint32>::max())
        {
            return false;
        }

        VkMemoryAllocateInfo MallocInfo{};
        MallocInfo.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        MallocInfo.allocationSize   = VSize;
        MallocInfo.memoryTypeIndex  = VMemoryTypeIndex;

        VK_CHECK(vkAllocateMemory(VDevice, &MallocInfo, nullptr, &VDeviceMemory));

        if (VDeviceMemory == VK_NULL_HANDLE)
        {
            return false;
        }

        if (isHostVisible())
        {
            VK_CHECK(vkMapMemory(VDevice, VDeviceMemory, 0, VSize, 0, reinterpret_cast<void**>(&VData)));
        }

        VHead           = new VVulkanChunk();
        VHead->id       = VNextBlockID++;
        VHead->size     = VSize;
        VHead->offset   = 0;
        VHead->previous = nullptr;
        VHead->next     = nullptr;
        VHead->type     = VAT_FREE;
        
        return true;
    }

    static int Vulkan::SetupVulkanRHIRenderer(vector<const char*> InstanceExtensions)
    {
        VkApplicationInfo AppInfo{};
        AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        AppInfo.pApplicationName = "Valhalla Engine";
        AppInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
        AppInfo.pEngineName = "Valhalla Engine";
        AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        AppInfo.apiVersion = VK_API_VERSION_1_3;

        /*Create Vulkan Instance*/
        VkInstanceCreateInfo InstanceInfo{};
        InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceInfo.pApplicationInfo = &AppInfo;
        
        /*We need a method of creating the window for Linux/Windows for vulkan*/
        

        /*Continuing the create vulkan instance*/
        InstanceInfo.enabledExtensionCount = static_cast<VkUint32>(InstanceExtensions.size());
        InstanceInfo.ppEnabledExtensionNames = InstanceExtensions.data();
        VK_CHECK(vkCreateInstance(&InstanceInfo, VAllocator, &VInstance));

        VkUint32 DeviceCount = 0;
        vkEnumeratePhysicalDevices(VInstance, &DeviceCount, nullptr);

        if (DeviceCount == 0)
        {
            ErrorMessageBox("Vulkan Error", "Failed to find Vulkan compatible GPU!");
            return -1;
        }
        fmt::print("Device Count: {}\n", DeviceCount);
        vector<VkPhysicalDevice> Devices(DeviceCount);
        VK_CHECK(vkEnumeratePhysicalDevices(VInstance, &DeviceCount, Devices.data()));

        for (const auto& Device : Devices)
        {
            if (IsDeviceSuitable(Device))
            {
                VPhysicalDevice = Device;
                break;
            }
        }

        if (VPhysicalDevice == VK_NULL_HANDLE)
        {
            ErrorMessageBox("Vulkan Error", "Failed to find a suitable GPU!");
            return -1;
        }

        vkGetPhysicalDeviceProperties(VPhysicalDevice, &VPhysicalDeviceProperties);
        fmt::print("Physical Device: {} - API Version: {}.{}.{}\n", VPhysicalDeviceProperties.deviceName, VK_VERSION_MAJOR(VPhysicalDeviceProperties.apiVersion), VK_VERSION_MINOR(VPhysicalDeviceProperties.apiVersion), VK_VERSION_PATCH(VPhysicalDeviceProperties.apiVersion));

        VVulkanQueueFamilyIndices indices = FindQueueFamilies(VPhysicalDevice);
        vector<VkDeviceQueueCreateInfo> QueueCreateInfo;
        set<VkUint32> UniqueQueueFamilies = { indices.GraphicsFamily, indices.PresentFamily };

        float QueuePriority = 1.0f;
        for (VkUint32 QueueFamily: UniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo QueueInfo{};
            QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            QueueInfo.queueFamilyIndex = QueueFamily;
            QueueInfo.queueCount = 1;
            QueueInfo.pQueuePriorities = &QueuePriority;
            QueueCreateInfo.push_back(QueueInfo);
        }
    }   

    void Vulkan::VulkanCreateSwapchainRHI()
    {
        VVulkanSwapChainSupportDetails SwapchainSupport = GetSwapchainSupport();
        VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapchainSupport.formats);
        VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapchainSupport.PresentModes);
        VkExtent2D Extent = ChooseSwapExtent(SwapchainSupport.capabilities);

        VkUint32 ImageCount = SwapchainSupport.capabilities.minImageCount + 1;
    }

    VVulkanQueueFamilyIndices Vulkan::FindQueueFamilies(VkPhysicalDevice device)
    {
        VVulkanQueueFamilyIndices Indices;

        VkUint32 QueueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamilyCount, nullptr);

        vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamilyCount, QueueFamilies.data());

        int i = 0;
        for (const auto& QueueFamily : QueueFamilies)
        {
            if (QueueFamily.queueCount > 0 && QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                Indices.GraphicsFamily = i;
				Indices.DoesGraphicsHasValue = true;
            }

            VkBool32 PresentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VSurface, &PresentSupport);

            if (QueueFamily.queueCount > 0 && PresentSupport)
            {
                Indices.PresentFamily = i;
                Indices.DoesPresentFamilyHasValue = true;
            }

            if (Indices.IsComplete())
            {
                break;
            }
            
            i++;
        }

        return Indices;
    }

    VVulkanSwapChainSupportDetails Vulkan::QuerySwapchainSupport(VkPhysicalDevice device)
    {
        VVulkanSwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, VSurface, &details.capabilities);

        VkUint32 FormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, VSurface, &FormatCount, nullptr);

        if (FormatCount != 0)
        {
            details.formats.resize(FormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, VSurface, &FormatCount, details.formats.data());
        }

        VkUint32 PresentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, VSurface, &PresentModeCount, nullptr);

        if (PresentModeCount != 0)
        {
            details.PresentModes.resize(PresentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, VSurface, &PresentModeCount, details.PresentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR Vulkan::ChooseSwapSurfaceFormat(const vector<VkSurfaceFormatKHR>& UsableFormats)
    {
        for (const auto& uformat : UsableFormats)
        {
            if (uformat.format == VK_FORMAT_B8G8R8A8_SRGB && uformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return uformat;
            }

            return UsableFormats[0];
        }
    }

    VkPresentModeKHR Vulkan::ChooseSwapPresentMode(const vector<VkPresentModeKHR>& UsablePresentModes)
    {
        for (const auto& upresent : UsablePresentModes)
        {
            if (upresent == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                fmt::print("Engine: Vulkan present mode = Mailbox\n");
                return upresent;
            }
        }

        fmt::print("Engine: Vulkan present mode = FIFO\n");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D Vulkan::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        VkUint32 Requiem = (numeric_limits<VkUint32>::max());

        if (capabilities.currentExtent.width != Requiem)
        {
            return capabilities.currentExtent;
        }
        else 
        {
            VkExtent2D ForetoldExtent = VWindowExtent;
            ForetoldExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, ForetoldExtent.width));
            ForetoldExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, ForetoldExtent.height));
            return ForetoldExtent;
        }
    }
}

VVulkanSwapchain::VVulkanSwapchain()
{

}

VVulkanSwapchain::~VVulkanSwapchain()
{
    for (auto ImageView : VSwapchainImageViews)
    {
        vkDestroyImageView(VDevice, ImageView, VAllocator);
    }

    VSwapchainImageViews.clear();

    if (VSwapchain != nullptr)
    {
        vkDestroySwapchainKHR(VDevice, VSwapchain, VAllocator);
        VSwapchain = nullptr;
    }

    for (int i = 0; i < VDepthImages.size(); i++)
    {
        vkDestroyImageView(VDevice, VDepthImageViews[i], nullptr);
        vkDestroyImage(VDevice, VDepthImages[i], nullptr);
        vkFreeMemory(VDevice, VDepthImagesMemories[i], nullptr);
    }

    for (auto framebuffer : VSwapchainFramebuffers)
    {
        vkDestroyFramebuffer(VDevice, framebuffer, nullptr);
    }

    vkDestroyRenderPass(VDevice, VRenderPass, nullptr);

    for (VkUsize i = 0; i < VSwapchainImages.size(); i ++)
    {
        vkDestroySemaphore(VDevice, VRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(VDevice, VImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(VDevice, VInFlightFences[i], nullptr);
    }
}

VVulkanPipeline::VVulkanPipeline(const string& VertexFilepath, const string& FragmentFilepath)
{

}

VVulkanPipeline::~VVulkanPipeline()
{
    
}