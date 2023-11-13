cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME 1-FractalViewer)

include_directories(Include)
include_directories(Sources)

set(INCLUDE
        Sources/main.cpp
        Sources/FractalView.cpp

        Sources/FractalRenderers/Mandelbrot.cpp

        Sources/MoveFractalBehaviours/MoveNearWindowSidesBehaviour.cpp
        Sources/MoveFractalBehaviours/DragBehaviour.cpp
)

set(SOURCES
        Include/FractalView.h

        Include/FractalRenderers/Mandelbrot.h
        Include/FractalRenderers/BaseFractalRenderer.h

        Include/MoveFractalBehaviours/BaseMoveBehaviour.h
        Include/MoveFractalBehaviours/MoveNearWindowSidesBehaviour.h
        Include/MoveFractalBehaviours/DragBehaviour.h
)

set (RESOURCES
        Shaders/mandelbrot.frag
        Shaders/mandelbrot.vert
        shaders.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${TARGET_NAME} ${SOURCES} ${INCLUDE} ${RESOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::Widgets
        FGL::Base
)