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

add_library(win_lib ${SRC} ${HEADER_FILES})

# Link glew
if(GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
endif()

target_link_libraries(win_lib DebugLogger glfw ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY})
target_include_directories(win_lib PUBLIC include nk_include)
