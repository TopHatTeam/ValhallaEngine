# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------

add_library(ValhallaMetalRHI STATIC)

target_sources(ValhallaMetalRHI PRIVATE
    v_metalbackend.mm
)

target_include_directories(ValhallaMetalRHI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaMetalRHI PUBLIC
    VALHALLA_ENGINE
)

