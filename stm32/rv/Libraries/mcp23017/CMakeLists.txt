cmake_minimum_required(VERSION 3.16)

add_library(mcp23017 STATIC src/mcp23017.c)
target_include_directories(mcp23017 PUBLIC include)