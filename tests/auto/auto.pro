QT += testlib xmlcompat xml
QT -= gui
greaterThan(QT_MAJOR_VERSION, 5) {
QT += core5compat
}

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle cmake

TEMPLATE = app

SOURCES +=  tst_qdomdocumentcompattest.cpp

DEFINES += QDOMDOCUMENTCOMPAT_LIBRARY_TEST
RESOURCES += tst_qdomdocumentcompattest.qrc


