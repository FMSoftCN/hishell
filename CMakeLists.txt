cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(HYBRIDOS_ROOT_PATH "$ENV{HYBRIDOS_OUTPUT_ROOT}")
INCLUDE_DIRECTORIES("${HYBRIDOS_ROOT_PATH}/include" "${HYBRIDOS_ROOT_PATH}/usr/include")
LINK_DIRECTORIES("${HYBRIDOS_ROOT_PATH}/lib" "${HYBRIDOS_ROOT_PATH}/usr/lib")

OPTION(PLATFORM_R818 "Compiled for platform R818" OFF)
if(PLATFORM_R818)
  MESSAGE(STATUS "Compiled for platform R818")
  ADD_DEFINITIONS(-DPLATFORM_R818)
endif()

OPTION(PLATFORM_PX30 "Compiled for platform PX30" OFF)
if(PLATFORM_PX30)
  MESSAGE(STATUS "Compiled for platform PX30")
  ADD_DEFINITIONS(-DPLATFORM_PX30)
endif()

find_package(MbedTLS REQUIRED)
find_package(Securec REQUIRED)
find_package(Jerryscript REQUIRED)
find_package(HiAceJS REQUIRED)

find_package(MiniGUI 5.0.2 REQUIRED)
find_package(mGEff 1.5.0 REQUIRED)
find_package(GLIB 2.0 REQUIRED)
if(!PLATFORM_PX30)
find_package(Cairo REQUIRED)
endif()
find_package(Librsvg 2.0 REQUIRED)
find_package(LibSoup 2.54.0 REQUIRED)
find_package(HiWebKit REQUIRED)

set(MINIGUI_LIBRARIES ${MiniGUI_LIBRARIES})
set(MINIGUI_INCLUDE_DIRS ${MiniGUI_INCLUDE_DIRS})

set(HIWEBKIT_INCLUDE_DIRS ${HiWebKit_INCLUDE_DIRS})
set(HIWEBKIT_LIBRARIES ${HiWebKit_LIBRARIES})

set(MGEFF_LIBRARIES ${mGEff_LIBRARIES})
set(MGEFF_INCLUDE_DIRS ${mGEff_INCLUDE_DIRS})

# copy the necessary resource files to /bin/res
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/res)

list(APPEND HISHELL_RES_FILES
    "${PROJECT_SOURCE_DIR}/dybkgnd/res/wallpaper-bottom.jpg"
    "${PROJECT_SOURCE_DIR}/dybkgnd/res/wallpaper-top.jpg"
#    "${PROJECT_SOURCE_DIR}/dybkgnd/res/home.svg"
#    "${PROJECT_SOURCE_DIR}/dybkgnd/res/arrow.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/arrow.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/home.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/toggle.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/setting.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/shutdown.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/about.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/wifi-0.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/wifi-1.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/wifi-2.svg"
    "${PROJECT_SOURCE_DIR}/appagent/res/wifi-3.svg"
)

file(COPY ${HISHELL_RES_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/bin/res)
file(COPY "MiniGUI.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
file(COPY "hybridos.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)
file(COPY "env.sh" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)

# set output path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set sub-directory
add_subdirectory(mginit)
add_subdirectory(appagent)
add_subdirectory(dybkgnd)
add_subdirectory(hiweb)
add_subdirectory(hijs)

install(FILES "MiniGUI.cfg" DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
install(FILES "hybridos.cfg" DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
install(DIRECTORY samples DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)

