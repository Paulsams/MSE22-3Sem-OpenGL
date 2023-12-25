cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

set(TARGET_NAME 4-Raytracing)

include_directories(Include)
include_directories(Sources)

file(INSTALL Models DESTINATION ./)

set(INCLUDE
        Include/Hittables/hittable.h
        Include/Hittables/hittable_list.h
        Include/Hittables/model.h
        Include/Hittables/sphere.h

        Include/LoaderModels/LoaderModel.h
        Include/LoaderModels/LoadModelIterator.h

        Include/Materials/material.h

        Include/Structures/vec3.h
        Include/Structures/color.h
        Include/Structures/ray.h
        Include/Structures/interval.h
        Include/Structures/triangle3.h

        Include/utils.h
        Include/camera.h

        Include/thread_pool_light.h
        Include/scenes.h
)

set(SOURCES
        Sources/LoaderModels/LoaderModel.cpp
        Sources/LoaderModels/LoadModelIterator.cpp

        Sources/main.cpp
)

set(RESOURCES
)

ADD_DEFINITIONS(-D_PROJECT_FILE_DIR="${PROJECT_SOURCE_DIR}")

add_executable(${TARGET_NAME} ${SOURCES} ${INCLUDE} ${RESOURCES})

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thirdparty::tinygltf
        thirdparty::glm
)
