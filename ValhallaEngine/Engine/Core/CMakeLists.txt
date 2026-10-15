# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------


add_library(ValhallaEngineCore SHARED)

if(APPLE)
target_sources(ValhallaEngineCore PRIVATE
    VPlatform.mm
    VMaterial.cpp
)
endif()

if(WIN32 AND UNIX)
target_sources(ValhallaEngineCore PRIVATE
    VPlatform.cpp
    VMaterial.cpp
)
endif()

target_include_directories(ValhallaEngineCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaEngineCore PUBLIC
    VALHALLA_ENGINE
)

target_link_libraries(ValhallaEngineCore PUBLIC
    fmt::fmt
)
