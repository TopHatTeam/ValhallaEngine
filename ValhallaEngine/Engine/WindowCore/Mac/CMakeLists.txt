

add_library(ValhallaEngineMacCore STATIC)

target_sources(ValhallaEngineMacCore PRIVATE
    VMac.mm
)

target_include_directories(ValhallaEngineMacCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaEngineMacCore PUBLIC
    VALHALLA_ENGINE
)

target_link_libraries(ValhallaEngineMacCore PUBLIC
    fmt::fmt
)
