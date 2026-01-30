add_library(ValhallaEngineEditor STATIC)

target_sources(ValhallaEngineEditor PRIVATE
    v_editor.cpp
)

target_include_directories(ValhallaEngineEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_definitions(ValhallaEngineEditor PUBLIC
    VALHALLA_ENGINE
)

target_link_libraries(ValhallaEngineEditor PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
)

