# Welcome to QtXmlCompat Project

## Overview

Reimplemented following functions of QDomDocument.

- `bool setContent(QXmlInputSource *source, QXmlReader *reader, QString *errorMsg=nullptr, int *errorLine=nullptr, int *errorColumn=nullptr );`
- `void save(QTextStream &s, int indent, EncodingPolicy encodingPolicy = QDomNode::EncodingFromDocument) const;`
- `QString toString(int indent = 1) const;`

Because SAX classes have been removed from Qt6.

Text nodes with whitespace are not removed when using this module.

- Input
    - `"<body> \n <p>abc<br/>  <span>def</span></p></body>"`
- By QDomDocument::setContent() -> save()
    - `"<body><p>abc<br/><span>def</span></p></body>"`
- By QDomDocumentCompat::setContent() -> save() *&lt;&lt;New>>*
    - `"<body> \n <p>abc<br/>  <span>def</span></p></body>"`

## Changes

It's intended to behave like the Qt5 version, but it has a little differences.


#### Attribute order (`QDomDocumentCompat::save()`)

In a debug build, the order of attributes is fixed.


#### Attribute and namespace order (`QDomDocumentCompat::save()`)

- QtXml(Qt5) : &lt;tag w:attr="data" xmlns:w="uri"/>
- QtXmlCompat : &lt;tag xmlns:w="uri" w:attr="data"/>

This is because the namespace will be lost if the XML file output by QXmlSimpleReader is repopulated.


## Supported Platforms

This module have been tested on the following platforms:
- Windows 10 (MSVC 2019 64bit)
- Ubuntu 20.04
- macOS 10.15


## How to use

### Requirements

- Qt 5.15.2
- Qt 6.0.0 ('Qt 5 Compatibility Module')
- Perl

### Building the module

#### qmake

```
$ git clone git@github.com:ioriayane/qtxmlcompat.git
$ mkdir build-qtxmlcompat
$ cd build-qtxmlcompat
$ qmake ../qtxmlcompat/qtxmlcompat.pro
$ make
$ make install
```

#### CMake

```
$ cmake -S qtxmlcompat -B build-qtxmlcompat -DCMAKE_PREFIX_PATH=~/Qt/6.4.3/macos -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build-qtxmlcompat
$ cmake --install build-qtxmlcompat
```

### Using the module

#### qmake project

```
QT += xmlcompat
greaterThan(QT_MAJOR_VERSION, 5) {
QT += core5compat
}
```

#### CMake project

```
find_package(QtXmlCompat CONFIG REQUIRED)

target_link_libraries(your_target PRIVATE QtXmlCompat::QtXmlCompat)
```

#### Source file(*.cpp/h)
```
#include <QtXmlCompat>
```

#### Example

main.cpp
```
#include <QtXmlCompat>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QDomDocumentCompat doc;
    QXmlInputSource xmlsource;
    QXmlSimpleReader xmlreader;
    QString xml = QStringLiteral("<body> \n <p>abc<br/>  <span>def</span></p></body>");

    xmlsource.setData(xml);
    doc.setContent(&xmlsource, &xmlreader);

    qDebug() << doc.toString(-1);

    return 0;
}
```

Output
```
"<body> \n <p>abc<br/>  <span>def</span></p></body>"
```

### Testing the module

Please run the test in a Debug build.

```
cmake --build build-qtxmlcompat --target tst_qdomdocumentcompattest
ctest --test-dir build-qtxmlcompat --output-on-failure
```
