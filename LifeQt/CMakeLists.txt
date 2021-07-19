cmake_minimum_required(VERSION 3.1.0)

project(primordial-life VERSION 4.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets Network Multimedia REQUIRED)

include_directories(.)

add_executable(primordial-life
    aboutui.ui
    dockviewbiot.ui
    mainwindow.ui
    networkui.ui
    settingsbiot.ui
    settingshabitat.ui
    settingsnetwork.ui
    settingsui.ui
    aboutui.cpp
    core/Biots.cpp
    core/Brain.cpp
    core/Connect.cpp
    core/Environ.cpp
    core/Etools.cpp
    core/Genotype.cpp
    core/Rand.cpp
    core/Settings.cpp
    core/SoundRegistry.cpp
    core/vector.cpp
    dockviewbiot.cpp
    environmentarea.cpp
    main.cpp
    mainwindow.cpp
    networking.cpp
    networkui.cpp
    settingsbiot.cpp
    settingshabitat.cpp
    settingsnetwork.cpp
    settingsui.cpp
    ui.qrc
)

target_link_libraries(primordial-life Qt5::Widgets Qt5::Network Qt5::Multimedia)

