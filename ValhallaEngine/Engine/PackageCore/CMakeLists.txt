# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------

add_library(ValhallaEngineVAP STATIC)

target_sources(ValhallaEngineVAP PRIVATE
    v_datapackage.cpp
)

target_include_directories(ValhallaEngineVAP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaEngineVAP PUBLIC
    VALHALLA_ENGINE
)

target_link_libraries(ValhallaEngineVAP PUBLIC
    fmt::fmt
)