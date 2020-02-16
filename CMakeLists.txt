cmake_minimum_required(VERSION 3.5)

project(SubControl-Lite LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
                    -W -Wall
)

find_package(Qt5 COMPONENTS Core Gui Widgets QuickWidgets Qml LinguistTools REQUIRED)

if (${Qt5_FOUND})
    message(STATUS "Found Qt " ${Qt5_VERSION})
else()
    message(FATAL_ERROR  "Couldn't find Qt")
endif()

link_libraries(
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::QuickWidgets
    Qt5::Qml
    )

qt5_wrap_ui(ui_list
    "ui/mainwindow.ui"
    )

qt5_wrap_cpp(moc_list
    "src/inc/mainwindow.h"

    "src/inc/qFlightInstruments.h"

    "src/inc/VideoReceiver.h"
    )

qt5_add_resources(qrc_list
    "qrc/assets.qrc")

qt5_add_translation(ts_list
    "translation/en_AU.ts")

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
pkg_search_module(GST REQUIRED gstreamer-1.0)
pkg_search_module(GST_VIDEO REQUIRED gstreamer-video-1.0)

include_directories(
    ${GLIB_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS} ${GST_VIDEO_INCLUDE_DIRS}
    "src/inc/"
    )

link_libraries(
    ${GLIB_LDFLAGS}
    ${GST_LDFLAGS} ${GST_VIDEO_LDFLAGS}
    )

set(src_list
    "src/main.cpp"
    "src/mainwindow.cpp"

    "src/qFlightInstruments.cpp"

    "src/VideoReceiver.cpp"
    )

add_subdirectory("ardusub_api")
include_directories("ardusub_api/api/inc")
link_libraries(ardusub_static)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
add_executable(${PROJECT_NAME} ${src_list} ${ui_list} ${moc_list} ${qrc_list})
else ()
add_executable(${PROJECT_NAME} WIN32 ${src_list} ${ui_list} ${moc_list} ${qrc_list})
endif ()

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
