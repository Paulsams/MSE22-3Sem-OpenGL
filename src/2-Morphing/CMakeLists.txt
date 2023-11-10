set(TARGET_NAME 2-Morphing)

include_directories(Include)
include_directories(Sources)

file(INSTALL Models DESTINATION ./)

set(SRCS
    Sources/main.cpp
    Sources/MorphingView.cpp
    Sources/Camera.cpp
    Sources/CameraView.cpp

    Include/MorphingView.h
    Include/Camera.h
    Include/CameraView.h

    Shaders/diffuse.frag
    Shaders/diffuse.vert
    Textures/voronoi.png

    resources.qrc
        Sources/Renderer.cpp
        Include/Renderer.h
        Sources/LoaderModel.cpp
        Include/LoaderModel.h
        Sources/SceneNode.cpp
        Include/SceneNode.h
        Sources/LoadModelIterator.cpp
        Include/LoadModelIterator.h
        Sources/Scene.cpp
        Include/Scene.h
        Sources/Transform.cpp
        Include/Transform.h
        Sources/MoverOnArrows.cpp
        Include/MoverOnArrows.h
        Include/BaseComponent.h
        Sources/InputHandler.cpp
        Include/InputHandler.h
        Sources/OpenGLUniformBuffer.cpp
        Sources/OpenGLUniformBuffer.h
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${TARGET_NAME} ${SRCS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
)

#add_custom_target(Models ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${. Input_DIR} ${OUTPUT_DIR}).
