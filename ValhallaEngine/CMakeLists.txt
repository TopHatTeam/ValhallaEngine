# -------------------------------------------------
#
#  MIT LICENSE
#
#  CMakeLists.txt
#
# -------------------------------------------------

cmake_minimum_required(VERSION 3.16)

if(WIN32)
    project(ValhallaEngine VERSION 1.0.0 LANGUAGES C CXX)
endif()

if(UNIX AND NOT APPLE)
    project(ValhallaEngine VERSION 1.0.0 LANGUAGES C CXX)
endif()

if(APPLE)
    project(ValhallaEngine VERSION 1.0.0 LANGUAGES C CXX OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(Qt6_DIR "$ENV{HOME}/Qt/6.10.1/macos/lib/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Quick)

if(WIN32)
    find_package(Vulkan REQUIRED)
endif()

if(UNIX AND NOT APPLE)
    find_package(Vulkan REQUIRED)
endif()

qt_standard_project_setup()

add_subdirectory(extern/fmt)    # fmt
add_subdirectory(Engine)
add_subdirectory(Engine/Editor)

if(APPLE)
    add_subdirectory(Engine/GraphicBackend/Metal)
    add_subdirectory(Engine/WindowCore/Mac)
endif()

if(APPLE)
add_executable(${PROJECT_NAME}
    Engine/Editor/v_editor.cpp
    Engine/Editor/headers/v_editor.hpp
    main.mm
)
endif()

if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6:Quick
    Vulkan::Vulkan
)
endif()

if(UNIX AND NOT APPLE)
target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
    Vulkan::Vulkan
)
endif()

if(APPLE)
target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
)
endif()
