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

# Generate mkspecs/modules .pri files (macOS)
if(APPLE)
    set(QT_LIB_XMLCOMPAT_PRI
"QT.xmlcompat.VERSION = ${PROJECT_VERSION}
QT.xmlcompat.name = QtXmlCompat
QT.xmlcompat.module = QtXmlCompat
QT.xmlcompat.ldflags = 
QT.xmlcompat.libs = $$QT_MODULE_LIB_BASE
QT.xmlcompat.includes = $$QT_MODULE_LIB_BASE/QtXmlCompat.framework/Headers
QT.xmlcompat.frameworks = $$QT_MODULE_LIB_BASE
QT.xmlcompat.bins = $$QT_MODULE_BIN_BASE
QT.xmlcompat.depends = core
QT.xmlcompat.uses =
QT.xmlcompat.module_config = v2 lib_bundle
QT.xmlcompat.DEFINES = QT_XMLCOMPAT_LIB
QT.xmlcompat.enabled_features =
QT.xmlcompat.disabled_features =
QT_CONFIG +=
QT_MODULES += xmlcompat
")

    set(QT_LIB_XMLCOMPAT_PRIVATE_PRI
"QT.xmlcompat_private.VERSION = ${PROJECT_VERSION}
QT.xmlcompat_private.name = QtXmlCompat
QT.xmlcompat_private.module =
QT.xmlcompat_private.libs = $$QT_MODULE_LIB_BASE
QT.xmlcompat_private.includes = $$QT_MODULE_LIB_BASE/QtXmlCompat.framework/Headers/${PROJECT_VERSION} $$QT_MODULE_LIB_BASE/QtXmlCompat.framework/Headers/${PROJECT_VERSION}/QtXmlCompat
QT.xmlcompat_private.frameworks =
QT.xmlcompat_private.depends = xmlcompat
QT.xmlcompat_private.uses =
QT.xmlcompat_private.module_config = v2 lib_bundle internal_module
QT.xmlcompat_private.enabled_features =
QT.xmlcompat_private.disabled_features =
")

    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt_lib_xmlcompat.pri"
        CONTENT "${QT_LIB_XMLCOMPAT_PRI}"
    )
    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt_lib_xmlcompat_private.pri"
        CONTENT "${QT_LIB_XMLCOMPAT_PRIVATE_PRI}"
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/qt_lib_xmlcompat.pri"
        "${CMAKE_CURRENT_BINARY_DIR}/qt_lib_xmlcompat_private.pri"
        DESTINATION mkspecs/modules
    )

    # Generate pkgconfig file
    set(PC_FILE_CONTENT "prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=\${prefix}
libdir=\${prefix}/lib
includedir=\${prefix}/lib/QtXmlCompat.framework/Headers

Name: Qt${QT_VERSION_MAJOR} XmlCompat
Description: Qt XmlCompat module
Version: ${PROJECT_VERSION}
Libs: -F\${libdir} -framework QtXmlCompat 
Cflags: -DQT_XMLCOMPAT_LIB -I\${includedir} -F\${libdir}
Requires: Qt${QT_VERSION_MAJOR}Core

")

    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Qt${QT_VERSION_MAJOR}XmlCompat.pc"
        CONTENT "${PC_FILE_CONTENT}"
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/Qt${QT_VERSION_MAJOR}XmlCompat.pc"
        DESTINATION lib/pkgconfig
    )
endif()

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