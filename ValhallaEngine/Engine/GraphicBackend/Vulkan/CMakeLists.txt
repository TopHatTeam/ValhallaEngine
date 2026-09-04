# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------

add_library(ValhallaEngineVulkanRHI SHARED)

target_sources(ValhallaEngineVulkanRHI PRIVATE
	v_vulkanbackend.cpp
)

target_include_directories(ValhallaEngineVulkanRHI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaEngineVulkanRHI PUBLIC
    VALHALLA_ENGINE
)

target_link_libraries(ValhallaEngineVulkanRHI PUBLIC
    fmt::fmt
    Vulkan::Vulkan
)