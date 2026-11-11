#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <vulkan/vulkan.h>

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>

static bool hasInstanceExtension(const char* name) {
    uint32_t count = 0;

    if (vkEnumerateInstanceExtensionProperties(
            nullptr, &count, nullptr) != VK_SUCCESS)
        return false;

    std::vector<VkExtensionProperties> extensions(count);

    if (vkEnumerateInstanceExtensionProperties(
            nullptr, &count, extensions.data()) != VK_SUCCESS)
        return false;

    for (const auto& ext : extensions) {
        if (std::strcmp(ext.extensionName, name) == 0)
            return true;
    }

    return false;
}

static bool hasDeviceExtension(
        VkPhysicalDevice device, const char* name) {
    uint32_t count = 0;

    if (vkEnumerateDeviceExtensionProperties(
            device, nullptr, &count, nullptr) != VK_SUCCESS)
        return false;

    std::vector<VkExtensionProperties> extensions(count);

    if (vkEnumerateDeviceExtensionProperties(
            device, nullptr, &count, extensions.data()) != VK_SUCCESS)
        return false;

    for (const auto& ext : extensions) {
        if (std::strcmp(ext.extensionName, name) == 0)
            return true;
    }

    return false;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_me_generalsx_vulkanprobe_MainActivity_nativeRun(
        JNIEnv* env, jobject, jobject javaSurface) {

    std::ostringstream out;

    ANativeWindow* window =
        ANativeWindow_fromSurface(env, javaSurface);

    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    do {
        if (!window) {
            out << "ERROR: Android surface unavailable.";
            break;
        }

        out << "Vulkan Android test\n\n";

        if (!hasInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME)) {
            out << "FAIL: VK_KHR_surface unavailable.";
            break;
        }

        if (!hasInstanceExtension(
                VK_KHR_ANDROID_SURFACE_EXTENSION_NAME)) {
            out << "FAIL: VK_KHR_android_surface unavailable.";
            break;
        }

        const char* instanceExtensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
        };

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Probe";
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType =
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = 2;
        instanceInfo.ppEnabledExtensionNames = instanceExtensions;

        VkResult result =
            vkCreateInstance(&instanceInfo, nullptr, &instance);

        if (result != VK_SUCCESS) {
            out << "FAIL: vkCreateInstance = " << result;
            break;
        }

        VkAndroidSurfaceCreateInfoKHR surfaceInfo{};
        surfaceInfo.sType =
            VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.window = window;

        result = vkCreateAndroidSurfaceKHR(
            instance, &surfaceInfo, nullptr, &surface);

        if (result != VK_SUCCESS) {
            out << "FAIL: Android surface creation = " << result;
            break;
        }

        uint32_t physicalCount = 0;

        result = vkEnumeratePhysicalDevices(
            instance, &physicalCount, nullptr);

        if (result != VK_SUCCESS || physicalCount == 0) {
            out << "FAIL: No Vulkan GPU found.";
            break;
        }

        std::vector<VkPhysicalDevice> physicalDevices(physicalCount);

        vkEnumeratePhysicalDevices(
            instance, &physicalCount, physicalDevices.data());

        VkPhysicalDevice physical = VK_NULL_HANDLE;
        uint32_t graphicsFamily = UINT32_MAX;
        uint32_t presentFamily = UINT32_MAX;

        bool swapchainFound = false;

        for (auto candidate : physicalDevices) {
            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(candidate, &properties);

            out << "GPU: " << properties.deviceName << "\n";

            if (!hasDeviceExtension(
                    candidate, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
                continue;

            swapchainFound = true;

            uint32_t queueCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(
                candidate, &queueCount, nullptr);

            std::vector<VkQueueFamilyProperties> queues(queueCount);

            vkGetPhysicalDeviceQueueFamilyProperties(
                candidate, &queueCount, queues.data());

            uint32_t graphics = UINT32_MAX;
            uint32_t present = UINT32_MAX;

            for (uint32_t i = 0; i < queueCount; i++) {
                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    graphics = i;

                VkBool32 supported = VK_FALSE;

                vkGetPhysicalDeviceSurfaceSupportKHR(
                    candidate, i, surface, &supported);

                if (supported)
                    present = i;
            }

            if (graphics != UINT32_MAX &&
                present != UINT32_MAX) {
                physical = candidate;
                graphicsFamily = graphics;
                presentFamily = present;
                break;
            }
        }

        if (!swapchainFound) {
            out << "FAIL: VK_KHR_swapchain unavailable.";
            break;
        }

        if (physical == VK_NULL_HANDLE) {
            out << "FAIL: No graphics and presentation queues.";
            break;
        }

        out << "Swapchain extension: YES\n";
        out << "Graphics queue: YES\n";
        out << "Presentation queue: YES\n";

        float priority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> queueInfos;

        VkDeviceQueueCreateInfo graphicsQueue{};
        graphicsQueue.sType =
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicsQueue.queueFamilyIndex = graphicsFamily;
        graphicsQueue.queueCount = 1;
        graphicsQueue.pQueuePriorities = &priority;

        queueInfos.push_back(graphicsQueue);

        if (presentFamily != graphicsFamily) {
            VkDeviceQueueCreateInfo presentQueue = graphicsQueue;
            presentQueue.queueFamilyIndex = presentFamily;
            queueInfos.push_back(presentQueue);
        }

        const char* deviceExtensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount =
            static_cast<uint32_t>(queueInfos.size());
        deviceInfo.pQueueCreateInfos = queueInfos.data();
        deviceInfo.enabledExtensionCount = 1;
        deviceInfo.ppEnabledExtensionNames = deviceExtensions;

        result = vkCreateDevice(
            physical, &deviceInfo, nullptr, &device);

        if (result != VK_SUCCESS) {
            out << "FAIL: vkCreateDevice = " << result;
            break;
        }

        VkSurfaceCapabilitiesKHR caps{};

        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical, surface, &caps);

        if (result != VK_SUCCESS) {
            out << "FAIL: Surface capabilities = " << result;
            break;
        }

        uint32_t formatCount = 0;

        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical, surface, &formatCount, nullptr);

        if (formatCount == 0) {
            out << "FAIL: No surface formats.";
            break;
        }

        std::vector<VkSurfaceFormatKHR> formats(formatCount);

        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical, surface, &formatCount, formats.data());

        VkSurfaceFormatKHR chosenFormat = formats[0];

        for (const auto& format : formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM ||
                format.format == VK_FORMAT_R8G8B8A8_UNORM) {
                chosenFormat = format;
                break;
            }
        }

        VkExtent2D extent = caps.currentExtent;

        if (extent.width == UINT32_MAX) {
            extent.width = static_cast<uint32_t>(
                ANativeWindow_getWidth(window));
            extent.height = static_cast<uint32_t>(
                ANativeWindow_getHeight(window));

            extent.width = std::max(
                caps.minImageExtent.width,
                std::min(caps.maxImageExtent.width, extent.width));

            extent.height = std::max(
                caps.minImageExtent.height,
                std::min(caps.maxImageExtent.height, extent.height));
        }

        if (extent.width == 0 || extent.height == 0) {
            out << "FAIL: Invalid surface dimensions.";
            break;
        }

        VkImageUsageFlags usage = 0;

        if (caps.supportedUsageFlags &
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        else if (caps.supportedUsageFlags &
                 VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        else if (caps.supportedUsageFlags &
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
            usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

        if (usage == 0) {
            out << "FAIL: No supported swapchain image usage.";
            break;
        }

        uint32_t imageCount = caps.minImageCount + 1;

        if (caps.maxImageCount > 0 &&
            imageCount > caps.maxImageCount)
            imageCount = caps.maxImageCount;

        VkCompositeAlphaFlagBitsKHR alpha =
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        const VkCompositeAlphaFlagBitsKHR alphaOptions[] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
        };

        bool alphaFound = false;

        for (auto option : alphaOptions) {
            if (caps.supportedCompositeAlpha & option) {
                alpha = option;
                alphaFound = true;
                break;
            }
        }

        if (!alphaFound) {
            out << "FAIL: No supported composite alpha mode.";
            break;
        }

        VkSwapchainCreateInfoKHR swapInfo{};
        swapInfo.sType =
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapInfo.surface = surface;
        swapInfo.minImageCount = imageCount;
        swapInfo.imageFormat = chosenFormat.format;
        swapInfo.imageColorSpace = chosenFormat.colorSpace;
        swapInfo.imageExtent = extent;
        swapInfo.imageArrayLayers = 1;
        swapInfo.imageUsage = usage;
        swapInfo.imageSharingMode =
            graphicsFamily == presentFamily
                ? VK_SHARING_MODE_EXCLUSIVE
                : VK_SHARING_MODE_CONCURRENT;

        if (graphicsFamily != presentFamily) {
            swapInfo.queueFamilyIndexCount = 2;
            swapInfo.pQueueFamilyIndices =
                new uint32_t[2]{graphicsFamily, presentFamily};
        }

        swapInfo.preTransform = caps.currentTransform;
        swapInfo.compositeAlpha = alpha;
        swapInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapInfo.clipped = VK_TRUE;
        swapInfo.oldSwapchain = VK_NULL_HANDLE;

        result = vkCreateSwapchainKHR(
            device, &swapInfo, nullptr, &swapchain);

        if (graphicsFamily != presentFamily)
            delete[] swapInfo.pQueueFamilyIndices;

        if (result != VK_SUCCESS) {
            out << "FAIL: vkCreateSwapchainKHR = " << result;
            break;
        }

        uint32_t actualImages = 0;
        vkGetSwapchainImagesKHR(
            device, swapchain, &actualImages, nullptr);

        out << "\nRESULT: SUCCESS\n";
        out << "Android surface: CREATED\n";
        out << "Swapchain: CREATED\n";
        out << "Resolution: " << extent.width
            << " x " << extent.height << "\n";
        out << "Swapchain images: " << actualImages << "\n";
        out << "\nNote: Creation test only; no frame was presented.";

    } while (false);

    if (device != VK_NULL_HANDLE) {
        if (swapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(device, swapchain, nullptr);

        vkDestroyDevice(device, nullptr);
    }

    if (surface != VK_NULL_HANDLE && instance != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(instance, surface, nullptr);

    if (instance != VK_NULL_HANDLE)
        vkDestroyInstance(instance, nullptr);

    if (window)
        ANativeWindow_release(window);

    return env->NewStringUTF(out.str().c_str());
}
