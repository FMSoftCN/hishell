cmake_minimum_required(VERSION 3.10)

project(ace_appagent)

# ace_appagent
list(APPEND ace_appagent_INCLUDE_DIRECTORIES
)

list(APPEND ace_appagent_SOURCES
    src/main.c
)

include_directories(${ace_appagent_INCLUDE_DIRECTORIES})
add_executable(ace_appagent ${ace_appagent_SOURCES})
target_link_libraries(ace_appagent hiacejs ${SECUREC_LIBRARIES} ${JERRYSCRIPT_LIBRARIES} ${MINIGUI_LIBRARIES})
install(TARGETS ace_appagent DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
