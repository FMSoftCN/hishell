set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required(VERSION 3.10)
project(appagent)
aux_source_directory(. DIR_SRCS)
include_directories(${MINIGUI_INCLUDE_DIRS})

set(MINIBROWSER_DIR "${PROJECT_SOURCE_DIR}")

set(appagent_SOURCES
    ${MINIBROWSER_DIR}/Common.cpp
    ${MINIBROWSER_DIR}/BrowserWindow.cpp
    ${MINIBROWSER_DIR}/WebKitBrowserWindow.cpp
    ${MINIBROWSER_DIR}/main.cpp
)

set(appagent_INCLUDE_DIRECTORIES
    ${MINIBROWSER_DIR}
)

set(appagent_SYSTEM_INCLUDE_DIRECTORIES
    ${GLIB_INCLUDE_DIRS}
    ${LIBSOUP_INCLUDE_DIRS}
#    ${GSTREAMER_INCLUDE_DIRS}
    ${Cairo_INCLUDE_DIR}
    ${MINIGUI_INCLUDE_DIRS}
    ${HIWEBKIT_INCLUDE_DIRS}
)

set(appagent_LIBRARIES
    ${MINIGUI_LIBRARIES}
    gobject-2.0
    icui18n
    icuuc
    icudata
    ${GLIB_LIBRARIES}
    ${LIBSOUP_LIBRARIES}
#    ${GSTREAMER_LIBRARIES}
    ${HIWEBKIT_LIBRARIES}
)


add_definitions(-DHBD_VERSION_MIN_REQUIRED=HBD_VERSION_1_0)
add_definitions(-DBUILDING_HBD__=1 -DBUILDING_WITH_CMAKE=1)

include_directories(${appagent_INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${appagent_SYSTEM_INCLUDE_DIRECTORIES})
add_executable(appagent ${appagent_SOURCES})
target_link_libraries(appagent ${appagent_LIBRARIES})

#WEBKIT_ADD_TARGET_CXX_FLAGS(appagent -Wno-unused-parameter)
target_compile_options(appagent PRIVATE -Wno-unused-parameter})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")

install(TARGETS appagent DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
