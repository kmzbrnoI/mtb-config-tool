cmake_minimum_required(VERSION 3.5)

project(mtb-config VERSION 0.1 LANGUAGES CXX)
configure_file(src/version.h.in version.h)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui ui/modules)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network LinguistTools)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES cz)

qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} mtb-config_cz.ts)

set(PROJECT_SOURCES
    src/main.h src/main.cpp
    src/q-str-exception.h
    src/client.h src/client.cpp
    src/settings.h src/settings.cpp
    src/log.h src/log.cpp
    src/win/win_main.h src/win/win_main.cpp
    src/win/win_settings.h src/win/win_settings.cpp
    src/win/win_log.h src/win/win_log.cpp
    src/win/win_mtbusb.h src/win/win_mtbusb.cpp
    src/common.h src/common.cpp
    src/win/modules/win_config.h
    src/win/modules/win_mtbuniconfig.h src/win/modules/win_mtbuniconfig.cpp
    ui/mainwindow.ui
    ui/settingsdialog.ui
    ui/logdialog.ui
    ui/mtbusbdialog.ui
    ui/modules/mtbuniconfig.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(mtb-config
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
else()
    add_executable(mtb-config
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
endif()

qt_add_translations(mtb-config)

# Add generated qm files to the resources
qt_add_resources(mtb-config "qm"
    PREFIX "/qm"
    BASE ${CMAKE_CURRENT_BINARY_DIR}
    FILES ${QM_FILES}
)

target_link_libraries(mtb-config PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(mtb-config PRIVATE Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(mtb-config PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Because of version.h
target_include_directories(mtb-config PUBLIC
    "${PROJECT_BINARY_DIR}"
    src
    src/win
    src/win/modules
)

install(TARGETS mtb-config
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(mtb-config)
endif()
