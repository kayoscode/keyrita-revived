cmake_minimum_required(VERSION 3.26)

set(EXE_NAME "keyrita")
project(${EXE_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# External libs
include(FetchContent)

FetchContent_Declare(
    debug_logger
    GIT_REPOSITORY  https://github.com/youngbrycecode/DebugLogger.git
    GIT_TAG         release1.0
)

FetchContent_Populate(debug_logger)
message(STATUS "debug_logger loc:" ${debug_logger_SOURCE_DIR})

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.9
)

FetchContent_Populate(glfw3)
message(STATUS "glfw loc:" ${glfw3_SOURCE_DIR})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

FetchContent_GetProperties(debug_logger)
add_subdirectory(${debug_logger_SOURCE_DIR})
FetchContent_GetProperties(glfw3)
add_subdirectory(${glfw3_SOURCE_DIR})
add_subdirectory(gui)
add_subdirectory(xml-parser)

file (GLOB SRC
    "*.cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
)

add_executable(${EXE_NAME} ${SRC} ${HEADER_FILES})

target_include_directories(${EXE_NAME} PUBLIC include)
target_link_libraries(${EXE_NAME} wgui DebugLogger xml_parser)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy glew dll if on windows
if (WIN32)
	add_custom_command(TARGET ${EXE_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${glew_BINARY_DIR}/bin/glew-sharedd.dll "${CMAKE_CURRENT_BINARY_DIR}"
		COMMENT "Copy dll file to ${CMAKE_CURRENT_BINARY_DIR} directory" VERBATIM
	)
endif()
