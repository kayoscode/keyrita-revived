cmake_minimum_required(VERSION 3.26)

set(PROJ_NAME xml_parser)
project(${PROJ_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC 
    "*.cpp"
    "include/*.h"
)

add_library(${PROJ_NAME}
    ${SRC}
)

target_include_directories(${PROJ_NAME} PUBLIC include)