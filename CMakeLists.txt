cmake_minimum_required(VERSION 3.24)
project(CNOVA_DB)

set(CMAKE_CXX_STANDARD 17)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/cnova.rc")

add_executable(repair main.cpp classes.h datafiles.h)
add_executable(DB gui.cpp Sicogui.h ${APP_ICON_RESOURCE_WINDOWS})
