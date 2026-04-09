cmake_minimum_required(VERSION 3.22)

# Указываем папки с кастомными компонентами
set(EXTRA_COMPONENT_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/components
        ${CMAKE_CURRENT_SOURCE_DIR}/components/Screens
        ${CMAKE_CURRENT_SOURCE_DIR}/components/Screens/SSD1306
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(project_name)