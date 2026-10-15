# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------

add_library(ValhallaMetalRHI SHARED)

target_sources(ValhallaMetalRHI PRIVATE
    VMetalRHI.cpp
    VMetalbackend.mm
)

target_include_directories(ValhallaMetalRHI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaMetalRHI PUBLIC
    VALHALLA_ENGINE
)

target_link_libraries(ValhallaMetalRHI PUBLIC
    MetalCPPLibrary
)