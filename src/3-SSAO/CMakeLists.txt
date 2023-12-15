cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME 3-SSAO)

include_directories(Include)
include_directories(Sources)

file(INSTALL Models DESTINATION ./)

set(SOURCES
        Sources/main.cpp
        Sources/SSAOView.cpp
        Sources/GBuffer.cpp
)

set(INCLUDE
        Include/SSAOView.h
        Include/GBuffer.h
)

set(RESOURCES
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
