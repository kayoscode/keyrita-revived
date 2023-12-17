cmake_minimum_required(VERSION 3.26)

set(EXE_NAME "nuc_gui")
project(${EXE_NAME} LANGUAGES CXX)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

FetchContent_Declare(
    debug_logger
    GIT_REPOSITORY git@github.com:youngbrycecode/DebugLogger.git
    GIT_TAG         release1.0
)

FetchContent_Populate(debug_logger)
message(STATUS "debug_logger loc:" ${debug_logger_SOURCE_DIR})
add_subdirectory(${debug_logger_SOURCE_DIR})

list(APPEND CMAKE_PREFIX_PATH "./glew")
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY git@github.com:glfw/glfw.git
    GIT_TAG        3.3.9
)

FetchContent_Populate(glfw3)
message(STATUS "glfw loc:" ${glfw3_SOURCE_DIR})
add_subdirectory(${glfw3_SOURCE_DIR})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

file (GLOB SRC
    "*cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
    "include/*.hpp"
)

include_directories(include)
add_executable(${EXE_NAME} ${SRC} ${HEADER_FILES})

# Link glew
if(GLEW_FOUND)
    message(STATUS "GLEW LOCATION: " ${GLEW_INCLUDE_DIRS})
    message(STATUS "GLEW STATIC LIB LOCATION " ${GLEW_LIBRARIES})

    target_link_libraries(${EXE_NAME} 
        ${GLEW_LIBRARIES}
        ${OPENGL_gl_LIBRARY}
        glfw)
endif()

target_include_directories(${EXE_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(${EXE_NAME} DebugLogger)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
if(WIN32)
	file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/glew/glew32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()