cmake_minimum_required(VERSION 3.16)

project(QtXmlCompat VERSION 1.0.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Xml)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core5Compat)
endif()

# Default install prefix: Qt directory (derived from the located Qt package).
# Override with -DCMAKE_INSTALL_PREFIX=<path> if needed.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    get_filename_component(_qt_install_prefix
        "${Qt${QT_VERSION_MAJOR}Core_DIR}/../../.."
        ABSOLUTE)
    set(CMAKE_INSTALL_PREFIX "${_qt_install_prefix}"
        CACHE PATH "Install path prefix" FORCE)
    message(STATUS "Default install prefix set to Qt directory: ${CMAKE_INSTALL_PREFIX}")
endif()

add_subdirectory(src)

if(BUILD_TESTING)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    add_subdirectory(tests)
endif()

install(EXPORT QtXmlCompatTargets
    FILE QtXmlCompatTargets.cmake
    NAMESPACE QtXmlCompat::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtXmlCompat
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtXmlCompatConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QtXmlCompatConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtXmlCompat
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QtXmlCompatConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/QtXmlCompatConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/QtXmlCompatConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtXmlCompat
)