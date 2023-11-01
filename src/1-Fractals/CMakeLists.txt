set(TARGET_NAME 1-FractalViewer)

include_directories(Include)
include_directories(Sources)

set(SRCS
        Sources/main.cpp
        Sources/FractalView.cpp
        Include/FractalView.h
        Sources/FractalRenderers/Mandelbrot.cpp
        Sources/FractalRenderers/Mandelbrot.cpp
        Include/FractalRenderers/Mandelbrot.h
        Include/FractalRenderers/BaseFractalRenderer.h
        Include/MoveFractalBehaviours/BaseMoveBehaviour.h
        Sources/MoveFractalBehaviours/MoveNearWindowSidesBehaviour.cpp
        Include/MoveFractalBehaviours/MoveNearWindowSidesBehaviour.h
        Shaders/mandelbrot.frag
        Shaders/mandelbrot.vert
        Sources/MoveFractalBehaviours/DragBehaviour.cpp
        Include/MoveFractalBehaviours/DragBehaviour.h
        ../Base/FrameCounter.h
        ../Base/FrameCounter.cpp

        shaders.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(${TARGET_NAME} ${SRCS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::Widgets
        FGL::Base
)