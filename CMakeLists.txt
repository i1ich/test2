cmake_minimum_required(VERSION 3.0)
project(lab02)

include(cmake/ide.cmake)
include(cmake/platform.cmake)

add_executable(lab02 WIN32 src/main.c src/win.h src/win.c src/editor.h src/editor.c src/wndproc.h src/wndproc.c src/utils.h src/utils.c src/menu.h)
