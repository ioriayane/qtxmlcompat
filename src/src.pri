QT += xml
greaterThan(QT_MAJOR_VERSION, 5) {
QT += core5compat
}
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/qdomdocumentcompat.cpp

HEADERS += \
    $$PWD/qdomdocumentcompat.h \
    $$PWD/qdomdocumentcompat_p.h \
    $$PWD/qtxmlcompat_global.h

