add_executable(tst_qdomdocumentcompattest
    tst_qdomdocumentcompattest.cpp
    tst_qdomdocumentcompattest.qrc
)

target_compile_definitions(tst_qdomdocumentcompattest PRIVATE QDOMDOCUMENTCOMPAT_LIBRARY_TEST)

target_link_libraries(tst_qdomdocumentcompattest
    PRIVATE
        QtXmlCompat
        Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(tst_qdomdocumentcompattest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

add_test(NAME tst_qdomdocumentcompattest COMMAND tst_qdomdocumentcompattest)