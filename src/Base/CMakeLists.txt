cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME Base)

include_directories(Include)
include_directories(Sources)

set(INCLUDE
        Include/GLWidget.hpp
        Include/Views/FieldsDrawer.h

        Include/LoaderModels/LoaderModel.h
        Include/LoaderModels/LoadModelIterator.h

        Include/SceneLogic/BaseComponent.h
        Include/SceneLogic/SceneNode.h
        Include/SceneLogic/Scene.h
        Include/SceneLogic/Transform.h
        Include/SceneLogic/MoverOnArrows.h

        Include/SceneLogic/Camera/Camera.h
        Include/SceneLogic/Camera/CameraView.h
        Include/SceneLogic/Renderer.h

        Include/Utils/FrameCounter.h
        Include/Utils/Time.h
        Include/Utils/OpenGLUniformBuffer.h
        Include/Utils/InputHandler.h
        Include/Utils/ToggleSwitch.h

        Include/Views/SceneHierarchyView.h
        Include/Views/ItemInHierarchy.h
        Include/Views/InspectorView.h
)

set(SOURCES
        Sources/GLWidget.cpp
        Sources/Views/FieldsDrawer.cpp

        Sources/LoaderModels/LoaderModel.cpp
        Sources/LoaderModels/LoadModelIterator.cpp

        Sources/SceneLogic/Renderer.cpp
        Sources/SceneLogic/SceneNode.cpp
        Sources/SceneLogic/Scene.cpp
        Sources/SceneLogic/Transform.cpp
        Sources/SceneLogic/MoverOnArrows.cpp

        Sources/SceneLogic/Camera/Camera.cpp
        Sources/SceneLogic/Camera/CameraView.cpp

        Sources/Utils/FrameCounter.cpp
        Sources/Utils/Time.cpp
        Sources/Utils/OpenGLUniformBuffer.cpp
        Sources/Utils/InputHandler.cpp
        Sources/Utils/ToggleSwitch.cpp

        Sources/Views/SceneHierarchyView.cpp
        Sources/Views/ItemInHierarchy.cpp
        Sources/Views/InspectorView.cpp
)

add_library(${TARGET_NAME} ${INCLUDE} ${SOURCES})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::Widgets
        thirdparty::tinygltf
)

add_library(FGL::Base ALIAS Base)