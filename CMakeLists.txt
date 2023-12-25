cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(MSE23-OpenGL-Paulsams VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    # Set Optimization Flags
    set(CMAKE_CXX_FLAGS_DEBUG "/O2")

    # Use fast math
    add_compile_options(/fp:fast)

    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
    # disable warning for qt.
    add_compile_options(/wd5240)
    # disable `no-if-not-aligned`
    add_compile_options(/wd4324)
    # disable `'sprintf': This function or variable may be unsafe`
    add_compile_options(/wd4996)
    # disable: 'unused-parameter' : unreferenced formal parameter
    add_compile_options(/wd4100)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wno-unused-parameter)
#    add_compile_options(-Ofast -g)

    if (GCC)
        add_compile_options(-Wno-if-not-aligned)
    else()
        add_compile_options(-Wno-over-aligned)
    endif()
endif()

add_subdirectory(thirdparty)

include_directories(src)

add_subdirectory(src/4-Raytracing)

# For Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src/Base)
add_subdirectory(src/1-Fractals)
add_subdirectory(src/2-Morphing)
add_subdirectory(src/2.5-Deferred)
add_subdirectory(src/3-SSAO)
