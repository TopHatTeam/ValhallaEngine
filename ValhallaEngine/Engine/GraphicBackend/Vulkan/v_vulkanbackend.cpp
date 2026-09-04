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
        if (VMemoryTypeIndex == UINT64_MAX)
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
}

VVulkanPipeline::VVulkanPipeline(const string& VertexFilepath, const string& FragmentFilepath)
{

}

VVulkanPipeline::~VVulkanPipeline()
{
    
}