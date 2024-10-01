cmake_minimum_required(VERSION 3.23)
project(redteam)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
        library.cpp
        library.h
        )

add_library(redteam SHARED ${SOURCES})
add_executable(test_app test_app.cpp)
add_executable(service service.cpp)

target_link_libraries(test_app redteam)
