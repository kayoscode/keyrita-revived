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
