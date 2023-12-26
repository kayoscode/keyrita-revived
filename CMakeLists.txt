cmake_minimum_required(VERSION 3.20)

set(EXE_NAME "keyrita")
project(${EXE_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/ZI)
	set(CMAKE_CXX_FLAGS_DEBUG "/MDd /ZI /Ob0 /Od /RTC1")
endif()

# External libs
include(FetchContent)

FetchContent_Declare(
    debug_logger
    GIT_REPOSITORY  https://github.com/youngbrycecode/DebugLogger.git
    GIT_TAG         release1.0
)
FetchContent_Populate(debug_logger)
message(STATUS "debug_logger loc:" ${debug_logger_SOURCE_DIR})
FetchContent_GetProperties(debug_logger)
add_subdirectory(${debug_logger_SOURCE_DIR})

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.9
)
FetchContent_Populate(glfw3)
message(STATUS "glfw loc:" ${glfw3_SOURCE_DIR})
FetchContent_GetProperties(glfw3)
add_subdirectory(${glfw3_SOURCE_DIR})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)

add_subdirectory(gui)

add_executable(${EXE_NAME} "main.cpp")

target_include_directories(${EXE_NAME} PUBLIC include)
target_link_libraries(${EXE_NAME} 
	wgui 
	DebugLogger
)

if (WIN32)
	FetchContent_Declare(
		minhook
		GIT_REPOSITORY https://github.com/youngbrycecode/minhook_mirror.git
	)
	FetchContent_Populate(minhook)
	message(STATUS "minhook loc: " ${minhook_SOURCE_DIR})
	FetchContent_GetProperties(minhook)
	add_subdirectory(${minhook_SOURCE_DIR})
	target_link_libraries(${EXE_NAME} minhook)
endif()

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCE_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)
set(DESTINATION_RESOURCES_DIR ${CMAKE_CURRENT_BINARY_DIR}/res)
add_custom_target(copy_resources_target
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_RESOURCES_DIR}/gui ${DESTINATION_RESOURCES_DIR}/gui
    COMMENT "Copying gui resources to destination"
    VERBATIM
)

# Make the copy target part of the build process
add_dependencies(${EXE_NAME} copy_resources_target)
