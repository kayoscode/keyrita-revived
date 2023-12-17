file (GLOB SRC
    "*cpp"
)

file (GLOB HEADER_FILES
    "include/*.h"    
    "nk_include/*.h"
    "include/*.hpp"
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
  option(glew-cmake_BUILD_SHARED "Build the shared glew library" ON)
  set(glew-cmake_BUILD_SHARED ON)
  add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
message("GLEW include " ${glew_SOURCE_DIR}/include)
message("GLEW bin " ${glew_BINARY_DIR}/include)

add_library(win_lib ${SRC} ${HEADER_FILES})

# Link glew
include_directories(${glew_SOURCE_DIR}/include)

target_link_libraries(win_lib DebugLogger glfw libglew_shared ${OPENGL_gl_LIBRARY})
target_include_directories(win_lib PUBLIC include nk_include)

