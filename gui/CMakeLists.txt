cmake_minimum_required(VERSION 3.20)

set(PROJ_NAME wgui)
file (GLOB SRC
    "*.cpp"
	"Shaders/*.cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
	"include/Shaders/*.h"
    "nk_include/*.h"
)

FetchContent_GetProperties(debug_logger)
FetchContent_GetProperties(glfw3)

FetchContent_Declare(glew
	GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
	GIT_TAG glew-cmake-2.2.0
	GIT_PROGRESS TRUE
)

FetchContent_GetProperties(glew)
if(NOT glew_POPULATED)
	FetchContent_Populate(glew)
	option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF)
	set(glew-cmake_BUILD_SHARED OFF)
	add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR})
endif()
message("GLEW include " ${glew_SOURCE_DIR}/include)
message("GLEW bin " ${glew_BINARY_DIR}/include)

FetchContent_Declare(
    xmllib
    GIT_REPOSITORY  https://github.com/zeux/pugixml.git
    GIT_TAG         latest
)
FetchContent_Populate(xmllib)

FetchContent_GetProperties(xmllib)
message(STATUS "xml lib loc:" ${xmllib_SOURCE_DIR})
add_subdirectory(${xmllib_SOURCE_DIR} ${xmllib_BINARY_DIR})

# Add GL Math library
FetchContent_Declare(
    glmath
    GIT_REPOSITORY  https://github.com/youngbrycecode/GLMath.git
)
FetchContent_Populate(glmath)

FetchContent_GetProperties(glmath)
message(STATUS "glmath lib loc:" ${glmath_SOURCE_DIR})
add_subdirectory(${glmath_SOURCE_DIR} ${glmath_BINARY_DIR})

add_library(${PROJ_NAME} ${SRC} ${HEADER_FILES})

# Link glew
include_directories(${glew_SOURCE_DIR}/include)

target_link_libraries(${PROJ_NAME} DebugLogger 
								   glfw 
								   libglew_static 
								   pugixml-static
								   GLMath
								   ${OPENGL_gl_LIBRARY})
target_include_directories(${PROJ_NAME} PUBLIC include nk_include)

# Add tests for wgui
add_subdirectory(Tests)
