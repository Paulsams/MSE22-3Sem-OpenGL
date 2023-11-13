cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME 2-Morphing)

include_directories(Include)
include_directories(Sources)

file(INSTALL Models DESTINATION ./)

set(SOURCES
        Sources/Lights/PointLight.cpp
        Sources/Lights/DirectionalLight.cpp
        Sources/Lights/SpotLight.cpp

        Sources/main.cpp
        Sources/MorphingView.cpp
)

set(INCLUDE
        Include/Lights/LightsContainer.h
        Include/Lights/PointLight.h
        Include/Lights/DirectionalLight.h
        Include/Lights/BaseLight.h
        Include/Lights/SpotLight.h

        Include/MorphingView.h
)

set(RESOURCES
        Shaders/diffuse.frag
        Shaders/diffuse.vert
        Textures/voronoi.png

        resources.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${TARGET_NAME} ${SOURCES} ${INCLUDE} ${RESOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
)

#add_custom_target(Models ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${. Input_DIR} ${OUTPUT_DIR}).
