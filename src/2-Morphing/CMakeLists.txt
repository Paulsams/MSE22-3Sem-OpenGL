set(TARGET_NAME 2-Morphing)

include_directories(Include)
include_directories(Sources)

set(SRCS
    Sources/main.cpp
    Sources/MorphingView.cpp
    Sources/ModelHolder.cpp
    Sources/ModelHolderNew.cpp
    Sources/Camera.cpp
    Sources/CameraView.cpp

    Include/MorphingView.h
    Include/ModelHolder.h
    Include/Camera.h
    Include/CameraView.h

    Shaders/diffuse.frag
    Shaders/diffuse.vert
    Textures/voronoi.png

    resources.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${TARGET_NAME} ${SRCS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
)