cmake_minimum_required(VERSION 3.18)
project(fall2023)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} cmd/main.cpp
        internal/common.h
        internal/log.h
        internal/function.h
        internal/method.h
        internal/method_nelder_mead.h
        internal/method_nelder_mead.cpp
        internal/method_random_walk.cpp
        internal/method_random_walk.h
        internal/random.cpp
        internal/random.h
        cmd/cli.h
)

add_subdirectory(vendor/fmt-10.2.1)
find_package(fmt)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

