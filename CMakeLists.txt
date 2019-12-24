cmake_minimum_required(VERSION 3.14)
project(xcb_learning)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        src/main.cc
        )

target_link_libraries(${PROJECT_NAME}
        xcb
        )
