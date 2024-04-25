# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# This line was added because the ESP compiler annoyingly interprets what it considers 
# bad struct declaration practice (in terms of brackets) as an error instead of a warning
add_compile_options(-Wno-missing-braces) 

project(wifi-data-acquisiton-server)

