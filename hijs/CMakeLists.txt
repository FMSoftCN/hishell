cmake_minimum_required(VERSION 3.10)

project(hijs)

# hijs 
list(APPEND hijs_INCLUDE_DIRECTORIES
)

list(APPEND hijs_SOURCES
    src/main.c
)

include_directories(${hijs_INCLUDE_DIRECTORIES})
add_executable(hijs ${hijs_SOURCES})
target_link_libraries(hijs ${HIACEJS_LIBRARIES} ${SECUREC_LIBRARIES} ${JERRYSCRIPT_LIBRARIES} ${MINIGUI_LIBRARIES})
install(TARGETS hijs DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
