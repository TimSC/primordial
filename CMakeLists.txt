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

include_directories(LifeQt)

add_executable(primordial-life
    LifeQt/aboutui.ui
    LifeQt/dockviewbiot.ui
    LifeQt/mainwindow.ui
    LifeQt/networkui.ui
    LifeQt/settingsbiot.ui
    LifeQt/settingshabitat.ui
    LifeQt/settingsnetwork.ui
    LifeQt/settingsui.ui
    LifeQt/aboutui.cpp
    LifeQt/core/Biots.cpp
    LifeQt/core/Brain.cpp
    LifeQt/core/Connect.cpp
    LifeQt/core/Environ.cpp
    LifeQt/core/Etools.cpp
    LifeQt/core/Genotype.cpp
    LifeQt/core/Rand.cpp
    LifeQt/core/Settings.cpp
    LifeQt/core/SoundRegistry.cpp
    LifeQt/core/vector.cpp
    LifeQt/dockviewbiot.cpp
    LifeQt/environmentarea.cpp
    LifeQt/main.cpp
    LifeQt/mainwindow.cpp
    LifeQt/networking.cpp
    LifeQt/networkui.cpp
    LifeQt/settingsbiot.cpp
    LifeQt/settingshabitat.cpp
    LifeQt/settingsnetwork.cpp
    LifeQt/settingsui.cpp
    LifeQt/ui.qrc
)

target_link_libraries(primordial-life Qt5::Widgets Qt5::Network Qt5::Multimedia)

install(TARGETS primordial-life
  RUNTIME DESTINATION bin
)

