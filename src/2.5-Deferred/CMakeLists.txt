cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME 2.5-Deferred)

include_directories(Include)
include_directories(Sources)
include_directories(Base/Sources)
include_directories(Base/Sources)

file(INSTALL Models DESTINATION ./)

set(SOURCES
        Sources/main.cpp
        Sources/DeferredView.cpp
        Sources/GBuffer.cpp
)

set(INCLUDE
        Include/DeferredView.h
        Include/GBuffer.h
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
