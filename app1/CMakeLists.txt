cmake_minimum_required(VERSION 3.15)
project(app1)

set(CMAKE_CXX_STANDARD 11)

set(includes
    "include/configurator.h"
    "include/cpu.h"
    "include/mem.h"
    "include/router.h"
    "include/types.h"
)

set(sources
    "src/configurator.cpp"
    "src/cpu.cpp"
    "src/main.cpp"
    "src/mem.cpp"
    "src/router.cpp"
)


add_executable(${PROJECT_NAME}
    ${includes}
    ${sources}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        D:/project_systemC/systemC/systemc-2.3.2/src
        D:/project_systemC/rapidJson/rapidjson-master/include/rapidjson
        D:/project_systemC/app1/include
        D:/project_systemC/app1/src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        D:/systemC/systemc-2.3.2/src/cmake-build-debug
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        systemc.a
)
