cmake_minimum_required(VERSION 3.29.2)
project(Emulator VERSION 0.1.0 LANGUAGES C CXX)


set(
    SourceFiles
    src/Emulator.cpp 
    src/CPU.cpp 
    src/Memory.cpp
)
find_package(SDL2 REQUIRED)
include_directories(include ${SDL2_INCLUDE_DIRS})
add_library(${PROJECT_NAME} STATIC ${SourceFiles})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})