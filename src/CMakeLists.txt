add_library(QtXmlCompat SHARED)
add_library(QtXmlCompat::QtXmlCompat ALIAS QtXmlCompat)

target_sources(QtXmlCompat
    PRIVATE
        qdomdocumentcompat.cpp
        qdomdocumentcompat.h
        qdomdocumentcompat_p.h
        qtxmlcompat_global.h
        QtXmlCompat
)

target_include_directories(QtXmlCompat
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(QtXmlCompat PRIVATE QTXMLCOMPAT_LIBRARY)

target_link_libraries(QtXmlCompat
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Xml
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    target_link_libraries(QtXmlCompat PUBLIC Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

set_target_properties(QtXmlCompat PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(TARGETS QtXmlCompat
    EXPORT QtXmlCompatTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    qdomdocumentcompat.h
    qdomdocumentcompat_p.h
    qtxmlcompat_global.h
    QtXmlCompat
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)