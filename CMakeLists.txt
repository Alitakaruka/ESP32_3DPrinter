cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Указываем папки с кастомными компонентами
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/Libs
    ${CMAKE_CURRENT_SOURCE_DIR}/components/Screens
)

project(project_name)
