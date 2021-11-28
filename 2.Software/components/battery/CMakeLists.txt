# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "battery.c"
    INCLUDE_DIRS "include")
    # REQUIRES lvgl)
# include($ENV{IDF_PATH}/tools/cmake/project.cmake)


