cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME 2.5-Deferred)

include_directories(Include)
include_directories(Sources)

file(INSTALL Models DESTINATION ./)

set(SOURCES
        Sources/main.cpp
        Sources/DeferredView.cpp
        Sources/GBuffer.cpp

        Sources/Lights/PointLightsContainer.cpp
        Sources/Lights/DirectionalLightsContainer.cpp
        Sources/Lights/PointLight.cpp
        Sources/Lights/DirectionalLight.cpp
)

set(INCLUDE
        Include/DeferredView.h
        Include/GBuffer.h

        Include/Lights/BaseLightsContainer.h
        Include/Lights/PointLightsContainer.h
        Include/Lights/DirectionalLightsContainer.h
        Include/Lights/PointLight.h
        Include/Lights/DirectionalLight.h
)

set(RESOURCES
        Shaders/deferred.vert
        Shaders/deferred.frag
        Shaders/directionalLight.vert
        Shaders/directionalLight.frag
        Shaders/light.vert
        Shaders/null.frag
        Shaders/null.vert
        Shaders/pointLight.frag

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
