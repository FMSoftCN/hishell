cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_PATH "/home/xue/work/hybridos/device-side/Output/HBD/Root/include")
set(CMAKE_LIBRARY_PATH "/home/xue/work/hybridos/device-side/Output/HBD/Root/lib")

set(ENV{PKG_CONFIG_PATH}  "/home/xue/work/hybridos/device-side/Output/HBD/Root/lib/pkgconfig")

find_package(MbedTLS REQUIRED)
find_package(Securec REQUIRED)
find_package(Jerryscript REQUIRED)

find_package(MiniGUI 5.0.2 REQUIRED)
find_package(mGEff 1.5.0 REQUIRED)
find_package(GLIB 2.0 REQUIRED)
find_package(Cairo REQUIRED)
find_package(Librsvg 2.0 REQUIRED)
find_package(LibSoup 2.54.0 REQUIRED)
find_package(HiWebKit REQUIRED)

set(MINIGUI_LIBRARIES ${MiniGUI_LIBRARIES})
set(MINIGUI_INCLUDE_DIRS ${MiniGUI_INCLUDE_DIRS})

set(HIWEBKIT_INCLUDE_DIRS ${HiWebKit_INCLUDE_DIRS})
set(HIWEBKIT_LIBRARIES ${HiWebKit_LIBRARIES})

set(MGEFF_LIBRARIES ${mGEff_LIBRARIES})
set(MGEFF_INCLUDE_DIRS ${mGEff_INCLUDE_DIRS})

link_directories("/home/xue/work/hybridos/device-side/Output/HBD/Root/lib")

# copy the necessary resource files to /bin/res
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/res)

list(APPEND HISHELL_RES_FILES
    "${PROJECT_SOURCE_DIR}/dybkgnd/res/wallpaper-bottom.jpg"
    "${PROJECT_SOURCE_DIR}/dybkgnd/res/wallpaper-top.jpg"
#    "${PROJECT_SOURCE_DIR}/dybkgnd/res/home.svg"
#    "${PROJECT_SOURCE_DIR}/dybkgnd/res/arrow.svg"
    "${PROJECT_SOURCE_DIR}/sysmgr/res/arrow.svg"
    "${PROJECT_SOURCE_DIR}/sysmgr/res/home.svg"
    "${PROJECT_SOURCE_DIR}/sysmgr/res/toggle.svg"
    "${PROJECT_SOURCE_DIR}/sysmgr/res/setting.svg"
    "${PROJECT_SOURCE_DIR}/sysmgr/res/shutdown.svg"
    "${PROJECT_SOURCE_DIR}/sysmgr/res/about.svg"
)

file(COPY ${HISHELL_RES_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/bin/res)
file(COPY "MiniGUI.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
file(COPY "hybridos.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
file(COPY "env.sh" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)

# set output path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set sub-directory
add_subdirectory(mginit)
add_subdirectory(sysmgr)
add_subdirectory(dybkgnd)
add_subdirectory(appagent)
add_subdirectory(ace_appagent)

install(FILES "MiniGUI.cfg" DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
install(FILES "hybridos.cfg" DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
install(DIRECTORY samples DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)

