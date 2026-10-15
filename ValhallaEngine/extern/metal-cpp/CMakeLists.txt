cmake_minimum_required(VERSION 3.16)

project(MetalCPPLibrary VERSION 1.0 LANGUAGES CXX)

add_library(MetalCPPLibrary INTERFACE)

target_include_directories(MetalCPPLibrary INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Foundation>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Metal>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QuartzCore>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/AppKit>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MetalKit>
    $<INSTALL_INTERFACE:include>
)

if(APPLE)
    target_compile_features(MetalCPPLibrary INTERFACE cxx_std_17)

    target_link_libraries(MetalCPPLibrary INTERFACE
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
        "-framework AppKit"
        "-framework MetalKit"
    )
endif()