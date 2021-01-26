#ifndef QDOMDOCUMENTCOMPAT_P_H
#define QDOMDOCUMENTCOMPAT_P_H

#include "qtxmlcompat_global.h"


#include "qdomdocumentcompat.h"
#include <QXmlDefaultHandler>

struct ErrorInfo{
    QString message;
    int lineNumber;
    int columnNumber;
};

class QXmlSimpleHandler : public QXmlDefaultHandler
{
public:
    QXmlSimpleHandler(QDomDocument *doc, bool namespaceProcessing);

    //QXmlContentHandler
    void setDocumentLocator(QXmlLocator* locator) override;
    bool startDocument() override;
    bool endDocument() override;
    bool startPrefixMapping(const QString& prefix, const QString& uri) override;
    bool endPrefixMapping(const QString& prefix) override;
    bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts) override;
    bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName) override;
    bool characters(const QString& ch) override;
    bool ignorableWhitespace(const QString& ch) override;
    bool processingInstruction(const QString& target, const QString& data) override;
    bool skippedEntity(const QString& name) override;
    QString errorString() const override;

    //QXmlLexicalHandler
    bool startDTD(const QString& name, const QString& publicId, const QString& systemId) override;
    bool endDTD() override;
    bool startEntity(const QString& name) override;
    bool endEntity(const QString& name) override;
    bool startCDATA() override;
    bool endCDATA() override;
    bool comment(const QString& ch) override;

    //QXmlDTDHandler
    bool notationDecl(const QString& name, const QString& publicId, const QString& systemId) override;
    bool unparsedEntityDecl(const QString& name, const QString& publicId, const QString& systemId, const QString& notationName) override;

    //QXmlDeclHandler
    bool attributeDecl(const QString& eName, const QString& aName, const QString& type, const QString& valueDefault, const QString& value) override;
    bool internalEntityDecl(const QString& name, const QString& value) override;
    bool externalEntityDecl(const QString& name, const QString& publicId, const QString& systemId) override;

    //QXmlErrorHandler:
    bool warning(const QXmlParseException& exception) override;
    bool error(const QXmlParseException& exception) override;
    bool fatalError(const QXmlParseException& exception) override;

    //other
    const ErrorInfo &errorInfo() const;
private:
    QDomDocument *document;
    bool namespaceProcessing;
    QDomNode currentNode;
    bool in_cdata;

    ErrorInfo m_errorInfo;
    QString m_errorString;
};

#endif // QDOMDOCUMENTCOMPAT_P_H
