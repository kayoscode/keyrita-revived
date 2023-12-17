cmake_minimum_required(VERSION 3.26)

set(EXE_NAME "nuc_gui")
project(${EXE_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External libs
include(FetchContent)

FetchContent_Declare(
    debug_logger
    GIT_REPOSITORY git@github.com:youngbrycecode/DebugLogger.git
    GIT_TAG         release1.0
)

FetchContent_Populate(debug_logger)
message(STATUS "debug_logger loc:" ${debug_logger_SOURCE_DIR})

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY git@github.com:glfw/glfw.git
    GIT_TAG        3.3.8
)

FetchContent_Populate(glfw3)
message(STATUS "glfw loc:" ${glfw3_SOURCE_DIR})

#list(APPEND CMAKE_PREFIX_PATH "./glew")
#find_package(GLEW REQUIRED)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

FetchContent_GetProperties(debug_logger)
add_subdirectory(${debug_logger_SOURCE_DIR})
FetchContent_GetProperties(glfw3)
add_subdirectory(${glfw3_SOURCE_DIR})
add_subdirectory(gui)

file (GLOB SRC
    "*cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
    "include/*.hpp"
)

include_directories(include)
add_executable(${EXE_NAME} ${SRC} ${HEADER_FILES})

target_link_libraries(${EXE_NAME} DebugLogger wgui)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# Copy glew dll if on windows
if (WIN32)
	file (COPY ${glew_BINARY_DIR}/bin/glew-sharedd.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
