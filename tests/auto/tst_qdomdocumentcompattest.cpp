#include <QDomImplementation>
#include <QtTest>

#include "qdomdocumentcompat.h"

struct TestInfo {
    TestInfo(const QString &id, const int indent, const QString &actual, const QString &expected){
        this->id = id;
        this->actual = actual;
        this->expected = expected;
        this->indent = indent;
    }
    QString id;
    QString actual;
    QString expected;
    int indent;
};

class QDomDocumentCompatTest : public QObject
{
    Q_OBJECT

public:
    QDomDocumentCompatTest();
    ~QDomDocumentCompatTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_constructor();
    void test_simpleReader();
    void test_from_file();
    void test_save();

    QString toStringUseSimpleReader(const QString &xml, const int indent) const;
    QString toString(const QString &xml, const int indent) const;
    QString loadFile(const QString &path);
    void saveFile(const QString &path, const QString &data);
};

QDomDocumentCompatTest::QDomDocumentCompatTest()
{

}

QDomDocumentCompatTest::~QDomDocumentCompatTest()
{

}

void QDomDocumentCompatTest::initTestCase()
{

}

void QDomDocumentCompatTest::cleanupTestCase()
{

}

void QDomDocumentCompatTest::test_constructor()
{
    {
        QDomImplementation impl;
        QDomDocumentType type = impl.createDocumentType("type_name", "public_id", "system_id");
        QDomDocumentCompat doc(type);
        QVERIFY(doc.doctype().name() == "type_name");
        QVERIFY(doc.doctype().publicId() == "public_id");
        QVERIFY(doc.doctype().systemId() == "system_id");

        QDomDocumentCompat doc2(doc);
        QVERIFY(doc2.doctype().name() == "type_name");
        QVERIFY(doc2.doctype().publicId() == "public_id");
        QVERIFY(doc2.doctype().systemId() == "system_id");
    }

    {
        QDomDocumentCompat doc("test_type_name");
        QVERIFY(doc.doctype().name() == "test_type_name");
    }
}


void QDomDocumentCompatTest::test_simpleReader()
{
    QList<TestInfo> list;
    QString left;
    QString right;



    //characters
    list.append(TestInfo(QStringLiteral("white 1"), 0
                         , QStringLiteral("<html>\r\n<p>hoge</p>\r\n</html>\n")
                         , QStringLiteral("<html>&#xd;\n<p>hoge</p>&#xd;\n</html>\n")));
    list.append(TestInfo(QStringLiteral("white 2"), -1
                         , QStringLiteral("<html><head></head><body> \n <p>abc<br/>  <span>def</span></p></body></html>")
                         , QStringLiteral("<html><head/><body> \n <p>abc<br/>  <span>def</span></p></body></html>")));

    //
    list.append(TestInfo(QStringLiteral("?? 1"), 0
                         , QStringLiteral("<body><p>foo <?php echo $a; ?></p>\n")
                         , QStringLiteral("<body>\n<p>foo <?php echo $a; ?>\n</p>\n</body>\n")));


    //ns duplicate
    list.append(TestInfo(QStringLiteral("ns duplicate 1"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\" xmlns:cp=\"cp_ns\"><vt:lpstr xmlns:vt=\"fuga\" cp:c1=\"v1\" cp:c2=\"v2\">title</vt:lpstr></Properties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"fuga\" xmlns:cp=\"cp_ns\" cp:c1=\"v1\" cp:c2=\"v2\">title</vt:lpstr></Properties>")));
    list.append(TestInfo(QStringLiteral("ns duplicate 2"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"fuga\" xmlns:cp=\"cp_ns\" cp:c1=\"v1\" cp:c2=\"v2\">title</vt:lpstr></Properties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"fuga\" xmlns:cp=\"cp_ns\" cp:c1=\"v1\" cp:c2=\"v2\">title</vt:lpstr></Properties>")));

    //indent
    list.append(TestInfo(QStringLiteral("indent 1"), 0
                         , QStringLiteral("<html><head></head><body> \n <p>abc<br/>  <span>def</span></p></body></html>")
                         , QStringLiteral("<html>\n<head/>\n<body> \n <p>abc<br/>  <span>def</span>\n</p>\n</body>\n</html>\n")));
    list.append(TestInfo(QStringLiteral("indent 2"), 2
                         , QStringLiteral("<html><head></head><body> \n <p>abc<br/>  <span>def</span></p></body></html>")
                         , QStringLiteral("<html>\n  <head/>\n  <body> \n <p>abc<br/>  <span>def</span>\n    </p>\n  </body>\n</html>\n")));

    //DTD
    list.append(TestInfo(QStringLiteral("DTD 1"), -1
                         , QStringLiteral("<!DOCTYPE HTML>\n<html><head><title>camp</title></head><body>YURUCAMP</body></html>")
                         , QStringLiteral("<!DOCTYPE HTML>\n<html><head><title>camp</title></head><body>YURUCAMP</body></html>")));

    list.append(TestInfo(QStringLiteral("DTD 2"), -1
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<!DOCTYPE members [\n<!ELEMENT members (person+)>\n<!ELEMENT person (name, age)>\n<!ELEMENT name (#PCDATA|family)*>\n<!ELEMENT family (#PCDATA)>\n<!ELEMENT age (#PCDATA)>\n<!ATTLIST age born NOTATION (EARLY|NORMAL) #REQUIRED>\n<!NOTATION EARLY PUBLIC \"Born early\">\n<!NOTATION NORMAL PUBLIC \"Born normal\">\n<!ENTITY Shimarin \"Rin Sima\">\n]>\n<members>\n<person><name>Nadeshiko <family>Kagamihara</family></name><age born=\"NORMAL\">16</age></person>\n<person><name>&Shimarin;</name><age born=\"EARLY\">15</age></person>\n</members>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<!DOCTYPE members>\n<members>\n<person><name>Nadeshiko <family>Kagamihara</family></name><age born=\"NORMAL\">16</age></person>\n<person><name>Rin Sima</name><age born=\"EARLY\">15</age></person>\n</members>")));

    //CDATA
    list.append(TestInfo(QStringLiteral("CDATA 1"), -1
                         , QStringLiteral("<p>\n  <div><![CDATA[hoge<\"'>&fuga]]></div>\n  <div>\n    <![CDATA[\n    hoge<\"'>\n    &fuga\n    ]]>\n  </div>\n</p>")
                         , QStringLiteral("<p>\n  <div><![CDATA[hoge<\"'>&fuga]]></div>\n  <div>\n    <![CDATA[\n    hoge<\"'>\n    &fuga\n    ]]>\n  </div>\n</p>")));

    //comment
    list.append(TestInfo(QStringLiteral("comment 1"), -1
                         , QStringLiteral("<p><div><!-- c1 -->\n body</div><!-- c2 --><div> <!--  c3  --> </div></p>")
                         , QStringLiteral("<p><div><!-- c1 -->\n body</div><!-- c2 -->\n<div> <!--  c3  --> </div></p>")));


    //skippedEntity
    list.append(TestInfo(QStringLiteral("entity 1"), -1
                         , QStringLiteral("<p>&amp;&lt;>]>]]&gt;\"hoge\"\'fuga\'</p>")
                         , QStringLiteral("<p>&amp;&lt;>]>]]&gt;\"hoge\"\'fuga\'</p>")));
    list.append(TestInfo(QStringLiteral("entity 2"), -1
                         ,QStringLiteral("<p>&amp;&lg;</p>")
                         , QStringLiteral("<p>&amp;&lg;</p>")));

    list.append(TestInfo(QStringLiteral("entity 3"), -1
                         ,QStringLiteral("<p id=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;&apos;fuga&apos;\">&amp;&lg;&NotSubset;&colon;:</p>")
                         , QStringLiteral("<p id=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;'fuga'\">&amp;&lg;&NotSubset;&colon;:</p>")));
    list.append(TestInfo(QStringLiteral("entity 4"), -1
                         ,QStringLiteral("<p id='&amp;&lt;>]>]]&gt;&quot;hoge&quot;&apos;fuga&apos;'>&amp;&lg;&NotSubset;&colon;:</p>")
                         , QStringLiteral("<p id=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;'fuga'\">&amp;&lg;&NotSubset;&colon;:</p>")));

    //namespace
    list.append(TestInfo(QStringLiteral("ns 1"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"fuga\">title</vt:lpstr></Properties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"fuga\">title</vt:lpstr></Properties>")));
    list.append(TestInfo(QStringLiteral("ns 2"), -1
                         ,QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<cp:coreProperties xmlns:cp=\"http://cp\" xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"http://xsi\"><cp:revision>10</cp:revision><dcterms:created xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><cp:revision xmlns:cp=\"http://cp\">10</cp:revision><dcterms:created xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"http://xsi\" xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")));
    list.append(TestInfo(QStringLiteral("ns 3"), -1
                         ,QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<cp:coreProperties xmlns:cp=\"http://cp\" xmlns:xsi=\"http://xsi\"><cp:revision>10</cp:revision><created xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</created></cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><cp:revision xmlns:cp=\"http://cp\">10</cp:revision><created xmlns:xsi=\"http://xsi\" xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</created></cp:coreProperties>")));
    list.append(TestInfo(QStringLiteral("ns 4"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><dcterms:created xmlns:dcterms=\"http://dcterms\" xsi:type=\"dcterms:W3CDTF\" xmlns:xsi=\"http://xsi\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><dcterms:created xmlns:dcterms=\"http://dcterms\" type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")));
    list.append(TestInfo(QStringLiteral("ns 5"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><dcterms:created xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"http://xsi\" xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><dcterms:created xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"http://xsi\" xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")));
    list.append(TestInfo(QStringLiteral("ns 6"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;&apos;fuga&apos;\"><dcterms:created xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;&apos;fuga&apos;\" xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;'fuga'\"><dcterms:created xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"&amp;&lt;>]>]]&gt;&quot;hoge&quot;'fuga'\" xsi:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")));
    list.append(TestInfo(QStringLiteral("ns 7"), -1
                         ,QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n<cp:coreProperties xmlns:cp=\"http://cp\" xmlns:dcterms=\"http://dcterms\" xmlns:xsi=\"http://xsi\"><cp:revision>10</cp:revision><dcterms:created dcterms:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\"><cp:revision xmlns:cp=\"http://cp\">10</cp:revision><dcterms:created xmlns:dcterms=\"http://dcterms\" dcterms:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created></cp:coreProperties>")));
    list.append(TestInfo(QStringLiteral("ns 8"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\">\n<dcterms:created xmlns:dcterms=\"http://dcterms\" dcterms:type=\"W3CDTF\" cp:type=\"dcterms:W3CDTF\">2021-01-28T12:39:00Z</dcterms:created>\n</cp:coreProperties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<cp:coreProperties xmlns:cp=\"http://cp\">\n<dcterms:created xmlns:dcterms=\"http://dcterms\" xmlns:cp=\"http://cp\" cp:type=\"dcterms:W3CDTF\" dcterms:type=\"W3CDTF\">2021-01-28T12:39:00Z</dcterms:created>\n</cp:coreProperties>")));

    //encode
    list.append(TestInfo(QStringLiteral("encode 1"), -1
                         ,QStringLiteral("<p id=\"ho\r\n\tge\">hoge</p>")
                         , QStringLiteral("<p id=\"ho&#xd;&#xa;&#x9;ge\">hoge</p>")));
    list.append(TestInfo(QStringLiteral("encode 2"), -1
                         ,QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"f\ru\ng\ta\">title</vt:lpstr></Properties>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<Properties xmlns=\"hoge\"><vt:lpstr xmlns:vt=\"f&#xd;u&#xa;g&#x9;a\">title</vt:lpstr></Properties>")));

    //attribute order
    //This test is failed on release build.
    list.append(TestInfo(QStringLiteral("order 1"), -1
                         ,QStringLiteral("<p id=\"hoge\" css=\"foo\" class=\"fuga\">order</p>")
                         , QStringLiteral("<p class=\"fuga\" css=\"foo\" id=\"hoge\">order</p>")));
    list.append(TestInfo(QStringLiteral("order 2"), -1
                         ,QStringLiteral("<p class=\"fuga\" css=\"foo\" id=\"hoge\">order</p>")
                         , QStringLiteral("<p class=\"fuga\" css=\"foo\" id=\"hoge\">order</p>")));
    list.append(TestInfo(QStringLiteral("order 3"), -1
                         ,QStringLiteral("<p css=\"foo\" id=\"hoge\" class=\"fuga\">order</p>")
                         , QStringLiteral("<p class=\"fuga\" css=\"foo\" id=\"hoge\">order</p>")));


    for(const TestInfo &info : list){
        if(info.id == "DTD 2"){
//            qDebug() << info.id;
        }
        left = toStringUseSimpleReader(info.actual, info.indent);
        right = info.expected;

        if(left != right){
            qDebug().noquote().nospace() << "//---- left ---\n" << left << "\n";
            qDebug().noquote().nospace() << "//---- right ---\n" << right << "\n";
        }
        QVERIFY2(left == right, info.id.toUtf8());
    }
}



void QDomDocumentCompatTest::test_from_file()
{
    QList<TestInfo> list;
    QString left;
    QString right;

    const int count = 2;

    for(int i=0; i<count; i++){
        list.append(TestInfo(QStringLiteral("html %1").arg(i), -1
                             , QString(":/html/act/data%1.html").arg(i+1)
                             , QString(":/html/exp/data%1.html").arg(i+1)));
    }

    QStringList xml_names;
    xml_names.append(".rels");
    xml_names.append("[Content_Types].xml");
    xml_names.append("app.xml");
    xml_names.append("core.xml");
    xml_names.append("document.xml");
    xml_names.append("fontTable.xml");
    xml_names.append("numbering.xml");
    xml_names.append("settings.xml");
    xml_names.append("styles.xml");
    xml_names.append("theme1.xml");
    xml_names.append("webSettings.xml");
    for(int i=0; i<xml_names.length(); i++){
        list.append(TestInfo(QStringLiteral("xml %1").arg(i), -1
                             , QString(":/xml/act/%1").arg(xml_names.at(i))
                             , QString(":/xml/exp/%1").arg(xml_names.at(i))));
    }

    for(const TestInfo &info : list){
        left = toStringUseSimpleReader(loadFile(info.actual), info.indent);
        right = loadFile(info.expected);
        saveFile(info.expected , left);

        QVERIFY2(!left.isEmpty() && !right.isEmpty(), (info.id + info.expected).toUtf8());
        QVERIFY2(left == right, (info.id + info.expected).toUtf8());
    }
}

void QDomDocumentCompatTest::test_save()
{
    QList<TestInfo> list;
    QString left;
    QString right;

    //indent
    list.append(TestInfo(QStringLiteral("indent"), 0
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<!DOCTYPE members [\n<!ELEMENT members (person+)>\n<!ELEMENT person (name, age)>\n<!ELEMENT name (#PCDATA|family)*>\n<!ELEMENT family (#PCDATA)>\n<!ELEMENT age (#PCDATA)>\n<!ATTLIST age born NOTATION (EARLY|NORMAL) #REQUIRED>\n<!NOTATION EARLY PUBLIC \"Born early\">\n<!NOTATION NORMAL PUBLIC \"Born normal\">\n<!ENTITY Shimarin \"Rin Sima\">\n]>\n<members>\n<person><name>Nadeshiko <family>Kagamihara</family></name><age born=\"NORMAL\">16</age></person>\n<person><name>&Shimarin;</name><age born=\"EARLY\">15</age></person>\n</members>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<!DOCTYPE members [\n<!NOTATION EARLY PUBLIC 'Born early'>\n<!NOTATION NORMAL PUBLIC 'Born normal'>\n]>\n<members>\n<person>\n<name>Nadeshiko <family>Kagamihara</family>\n</name>\n<age born=\"NORMAL\">16</age>\n</person>\n<person>\n<name>Rin Sima</name>\n<age born=\"EARLY\">15</age>\n</person>\n</members>\n")));

    //DTD
    list.append(TestInfo(QStringLiteral("DTD 1"), -1
                         , QStringLiteral("<!DOCTYPE HTML>\n<html><head><title>camp</title></head><body>YURUCAMP</body></html>")
                         , QStringLiteral("<!DOCTYPE HTML>\n<html><head><title>camp</title></head><body>YURUCAMP</body></html>")));

    list.append(TestInfo(QStringLiteral("DTD 2"), -1
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<!DOCTYPE members [\n<!ELEMENT members (person+)>\n<!ELEMENT person (name, age)>\n<!ELEMENT name (#PCDATA|family)*>\n<!ELEMENT family (#PCDATA)>\n<!ELEMENT age (#PCDATA)>\n<!ATTLIST age born NOTATION (EARLY|NORMAL) #REQUIRED>\n<!NOTATION EARLY PUBLIC \"Born early\">\n<!NOTATION NORMAL PUBLIC \"Born normal\">\n<!ENTITY Shimarin \"Rin Sima\">\n]>\n<members>\n<person><name>Nadeshiko <family>Kagamihara</family></name><age born=\"NORMAL\">16</age></person>\n<person><name>&Shimarin;</name><age born=\"EARLY\">15</age></person>\n</members>")
                         , QStringLiteral("<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n<!DOCTYPE members [\n<!NOTATION EARLY PUBLIC 'Born early'>\n<!NOTATION NORMAL PUBLIC 'Born normal'>\n]>\n<members><person><name>Nadeshiko <family>Kagamihara</family></name><age born=\"NORMAL\">16</age></person><person><name>Rin Sima</name><age born=\"EARLY\">15</age></person></members>")));


    for(const TestInfo &info : list){
        if(info.id == "DTD 2"){
//            qDebug() << info.id;
        }
        left = toString(info.actual, info.indent);
        right = info.expected;

        if(left != right){
            qDebug().noquote().nospace() << "//---- left ---\n" << left << "\n";
            qDebug().noquote().nospace() << "//---- right ---\n" << right << "\n";
        }
        QVERIFY2(left == right, info.id.toUtf8());
    }

}


QString QDomDocumentCompatTest::toStringUseSimpleReader(const QString &xml, const int indent) const
{
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    QXmlInputSource xmlsource;
    QXmlSimpleReader xmlreader;

    //Don't use.
    //xmlreader.setFeature(QStringLiteral("http://qt-project.org/xml/features/report-start-end-entity"), true);

    QDomDocumentCompat doc;
    xmlsource.setData(xml);
    if(!doc.setContent(&xmlsource, &xmlreader, &errorMsg, &errorLine, &errorColumn)){
        qDebug().noquote().nospace() << (errorMsg + ", Line=" + QString::number(errorLine) + ", Column=" + QString::number(errorColumn));
    }

    return doc.toString(indent);
}

QString QDomDocumentCompatTest::toString(const QString &xml, const int indent) const
{
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QDomDocumentCompat doc;
    if(!doc.setContent(xml, true, &errorMsg, &errorLine, &errorColumn)){
        qDebug().noquote().nospace() << (errorMsg + ", Line=" + QString::number(errorLine) + ", Column=" + QString::number(errorColumn));
    }

    return doc.toString(indent);
}

QString QDomDocumentCompatTest::loadFile(const QString &path)
{
    QString ret;
    QFile file(path);
    if(file.open(QFile::ReadOnly)){
        QTextStream in(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        in.setCodec("UTF-8");
#endif
        ret = in.readAll();
        file.close();
    }
    return ret;
}

void QDomDocumentCompatTest::saveFile(const QString &path, const QString &data)
{
    QFileInfo file_info(QString(path).remove(QRegularExpression("^:\\/")));
    QDir dir(file_info.absoluteDir());
    if(!dir.exists()){
        dir.mkpath(dir.absolutePath());
    }

    QFile file(file_info.absoluteFilePath());
    if(file.open(QFile::WriteOnly)){
        QTextStream out(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        out.setCodec("UTF-8");
#endif
        out << data;
        file.close();
    }
}




QTEST_APPLESS_MAIN(QDomDocumentCompatTest)

#include "tst_qdomdocumentcompattest.moc"
