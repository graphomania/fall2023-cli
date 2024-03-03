cmake_minimum_required(VERSION 3.18)
project(fall2023)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

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
        ui/cli.h
        ui/gui.h
        ui/gui_settings.h
        ui/parse.h
        ui/gui_heatmap.h
        ui/gui_widgets.h
)

add_subdirectory(vendor/fmt-10.2.1)
find_package(fmt)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Charts)