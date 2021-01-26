#ifndef QDOMDOCUMENTCOMPAT_H
#define QDOMDOCUMENTCOMPAT_H

#include "qtxmlcompat_global.h"

#include <QHash>
#include <QTextStream>
#include <QtXml/QDomDocument>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlInputSource>
#else
#include <QtCore5Compat/QXmlSimpleReader>
#include <QtCore5Compat/QXmlInputSource>
#endif

class QXmlSimpleHandler;

class QTXMLCOMPAT_EXPORT QDomDocumentCompat : public QDomDocument
{
public:
    QDomDocumentCompat();
    explicit QDomDocumentCompat(const QString& name);
    explicit QDomDocumentCompat(const QDomDocumentType& doctype);
    QDomDocumentCompat(const QDomDocumentCompat& x);
    ~QDomDocumentCompat();

    using QDomDocument::setContent;
    bool setContent(QXmlInputSource *source, QXmlReader *reader, QString *errorMsg=nullptr, int *errorLine=nullptr, int *errorColumn=nullptr );
    void save(QTextStream &s, int indent, EncodingPolicy encodingPolicy = QDomNode::EncodingFromDocument) const;
    QString toString(int indent = 1) const;

private:
    QXmlSimpleHandler *handler;
    bool namespaceProcessing;

    void save(QTextStream &s, const QDomNode &node, int depth, int indent, const QHash<QString, QString> ns_hash = QHash<QString, QString>()) const;
    QString encodeAttributeValue(const QString &text) const;
};

#endif // QDOMDOCUMENTCOMPAT_H
