// #include <iostream>
// #include <vector>

// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>

// std::vector<const char*> InstLayer = {"VK_KHRONOS_LAYER_validation"};

// int Width = 1280;
// int Height = 720;

// VkInstance Instance;
// VkPhysicalDevice PhysDev;
// VkDevice Device;

// int main()
// {
//     glfwInit();

//     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//     glfwCreateWindow(Width, Height, "Vulkan Renderer", NULL, NULL);

//     uint32_t glfwExtCount = 0;
//     const char** glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);

//     VkInstanceCreateInfo InstanceCI{};
//     InstanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//     InstanceCI.enabledExtensionCount = glfwExtCount;
//     InstanceCI.ppEnabledExtensionNames = glfwExt;
//     InstanceCI.enabledLayerCount = InstLayer.size();
//     InstanceCI.ppEnabledLayerNames = InstLayer.data();

//     if(vkCreateInstance(&InstanceCI, nullptr, &Instance) != VK_SUCCESS)
//     {
//         throw std::runtime_error("Failed to create Instance");
//     }

//     uint32_t PDevCount = 0;
//     vkEnumeratePhysicalDevices(Instance, &PDevCount, nullptr);
//     VkPhysicalDevice PDevices[PDevCount];
//     vkEnumeratePhysicalDevices(Instance, &PDevCount, PDevices);

//     for(uint32_t i = 0; i < PDevCount; i++)
//     {
//         VkPhysicalDeviceProperties Props;
//         vkGetPhysicalDeviceProperties(PDevices[i], &Props);
//         if(Props.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
//         {
//             PhysDev = PDevices[i];
//             break;
//         }
//     }

//     uint32_t GraphicsFamily;

//     uint32_t FamilyCount;
//     vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &FamilyCount, nullptr);
//     std::vector<VkQueueFamilyProperties> FamProps(FamilyCount);
//     vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &FamilyCount, FamProps.data());

//     for(uint32_t i = 0; i < FamilyCount; i++)
//     {
//         if(FamProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
//         {
//             GraphicsFamily = i;
//             break;
//         }
//     }

//     VkDeviceQueueCreateInfo GraphicsQueueCI{};
//     GraphicsQueueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//     GraphicsQueueCI.queueFamilyIndex = GraphicsFamily;
//     GraphicsQueueCI.queueCount = 1;

//     VkDeviceCreateInfo DeviceCI{};
//     DeviceCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//     DeviceCI.queueCreateInfoCount = 1;
//     DeviceCI.pQueueCreateInfos = &GraphicsQueueCI;

//     if(vkCreateDevice(PhysDev, &DeviceCI, nullptr, &Device) != VK_SUCCESS)
//     {
//         throw std::runtime_error("Failed to create Device");
//     }
// }
