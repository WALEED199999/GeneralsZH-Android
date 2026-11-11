cmake_minimum_required(VERSION 3.22.1)

project(vulkan_probe LANGUAGES CXX)

add_library(vulkan_probe SHARED native-lib.cpp)

target_compile_features(vulkan_probe PRIVATE cxx_std_17)

target_compile_definitions(vulkan_probe PRIVATE VK_USE_PLATFORM_ANDROID_KHR)

target_link_libraries(
    vulkan_probe
    android
    log
    vulkan
)
