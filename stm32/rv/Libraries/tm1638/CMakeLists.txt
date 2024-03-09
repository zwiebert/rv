cmake_minimum_required(VERSION 3.16)

add_library(tm1638 STATIC src/tm1638.c src/dlb8.c)
target_include_directories(tm1638 PUBLIC include)