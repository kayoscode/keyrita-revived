cmake_minimum_required(VERSION 3.26)

set(EXE_NAME "nuc_gui")
project(${EXE_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external_libs)

list(APPEND CMAKE_PREFIX_PATH "./glew")
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

FetchContent_GetProperties(debug_logger)
add_subdirectory(${debug_logger_SOURCE_DIR})
FetchContent_GetProperties(glfw3)
add_subdirectory(${glfw3_SOURCE_DIR})
add_subdirectory(window)

file (GLOB SRC
    "*cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
    "include/*.hpp"
)

include_directories(include)
add_executable(${EXE_NAME} ${SRC} ${HEADER_FILES})

target_link_libraries(${EXE_NAME} DebugLogger win_lib)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
if(WIN32)
	file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/glew/glew32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
