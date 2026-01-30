# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------


add_library(ValhallaEngineCore STATIC)

if(APPLE)
target_sources(ValhallaEngineCore PRIVATE
    v_datapackage.cpp
    #v_metalbackend.mm
    v_platform.mm
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
