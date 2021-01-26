#include "qdomdocumentcompat.h"
#include "qdomdocumentcompat_p.h"

#include <QDebug>

QDomDocumentCompat::QDomDocumentCompat()
    : QDomDocument()
    , handler(nullptr)
    , namespaceProcessing(false)
{
}

QDomDocumentCompat::QDomDocumentCompat(const QString &name)
    : QDomDocument(name)
    , handler(nullptr)
    , namespaceProcessing(false)
{
}

QDomDocumentCompat::QDomDocumentCompat(const QDomDocumentType &doctype)
    : QDomDocument(doctype)
    , handler(nullptr)
    , namespaceProcessing(false)
{
}

QDomDocumentCompat::QDomDocumentCompat(const QDomDocumentCompat &x)
    : QDomDocument(x)
    , handler(nullptr)
    , namespaceProcessing(false)
{
}

QDomDocumentCompat::~QDomDocumentCompat()
{
    if(handler != nullptr){
        delete handler;
    }
}

bool QDomDocumentCompat::setContent(QXmlInputSource *source, QXmlReader *reader, QString *errorMsg, int *errorLine, int *errorColumn)
{
    clear();

    namespaceProcessing = reader->feature(QLatin1String("http://xml.org/sax/features/namespaces"))
        && !reader->feature(QLatin1String("http://xml.org/sax/features/namespace-prefixes"));

    if(handler != nullptr){
        delete handler;
    }
    handler = new QXmlSimpleHandler(this, namespaceProcessing);

    reader->setContentHandler(handler);
    reader->setLexicalHandler(handler);
    reader->setDTDHandler(handler);
    reader->setDeclHandler(handler);
    reader->setErrorHandler(handler);

    bool ok = reader->parse(source);
    if(!ok){
        if(errorMsg != nullptr){
            *errorMsg = handler->errorInfo().message;
        }
        if(errorLine != nullptr){
            *errorLine = handler->errorInfo().lineNumber;
        }
        if(errorColumn != nullptr){
            *errorColumn = handler->errorInfo().columnNumber;
        }
    }

    return ok;
}

void QDomDocumentCompat::save(QTextStream &s, int indent, QDomNode::EncodingPolicy encodingPolicy) const
{
    Q_UNUSED(encodingPolicy)

    save(s, *this, 0, indent);
}

QString QDomDocumentCompat::toString(int indent) const
{
    QString str;
    QTextStream s(&str, QIODevice::WriteOnly);
    save(s, *this, 0, indent);
    return str;
}


void QDomDocumentCompat::save(QTextStream &s, const QDomNode &node, int depth, int indent, const QHash<QString, QString> ns_hash) const
{
    //qDebug() << node.nodeType() << node.nodeName() << node.nodeValue();
    if(node.isAttr()){
        if(!node.namespaceURI().isEmpty()){
            if(node.parentNode().namespaceURI() == node.namespaceURI()){
                //No output uri if mine(attribute) uri is equal parent(tag) uri.
            }else if(ns_hash.contains(node.namespaceURI())){
                //duplicate
            }else{
                s << " xmlns:" << node.prefix()
                  << QStringLiteral("=\"") << encodeAttributeValue(node.namespaceURI()) << QStringLiteral("\"");
            }
            s << QLatin1Char(' ') << node.prefix() << QLatin1Char(':');
        }else{
            s << QLatin1Char(' ');
        }
        s << node.localName() << QStringLiteral("=\"") << encodeAttributeValue(node.nodeValue()) << QStringLiteral("\"");

    }else if(node.isCDATASection()){
        node.toCDATASection().save(s, indent);

    }else if(node.isComment()){
        node.toComment().save(s, indent);

    }else if(node.isDocument()){
        bool first = true;
        for(int i=0; i < node.childNodes().length(); i++){
            if(first && !node.childNodes().at(i).isProcessingInstruction()){
                save(s, node.toDocument().doctype(), 0, indent);
                first = false;
            }
            save(s, node.childNodes().at(i), depth, indent);
        }

    }else if(node.isDocumentFragment()){
        node.toDocumentFragment().save(s, indent);

    }else if(node.isDocumentType()){
        if(!node.toDocumentType().name().isEmpty()){
            s << QStringLiteral("<!DOCTYPE ") << node.toDocumentType().name();
            if(!node.toDocumentType().publicId().isEmpty()){
                s << QStringLiteral(" PUBLIC ") << node.toDocumentType().publicId();
                if(!node.toDocumentType().systemId().isEmpty()){
                    s << QStringLiteral(" ") << node.toDocumentType().systemId();
                }
            }else if(!node.toDocumentType().systemId().isEmpty()){
                s << QStringLiteral(" SYSTEM ") << node.toDocumentType().systemId();
            }

            if(node.toDocumentType().entities().length() > 0
                    || node.toDocumentType().notations().length() > 0){
                s << QStringLiteral(" [") << Qt::endl;

                QHash<QString, QDomNode> hash;
                QStringList keys;
                for(int i=0; i<node.toDocumentType().entities().length(); i++){
                    hash[node.toDocumentType().entities().item(i).nodeName()] = node.toDocumentType().entities().item(i);
                }
                keys = hash.keys();
#ifdef QT_DEBUG
                keys.sort();
#endif
                for(const QString &key: keys){
                    save(s, node.toDocumentType().entities().namedItem(key), 0, indent);
                }

                hash.clear();
                keys.clear();
                for(int i=0; i<node.toDocumentType().notations().length(); i++){
                    hash[node.toDocumentType().notations().item(i).nodeName()] = node.toDocumentType().notations().item(i);
                }
                keys = hash.keys();
#ifdef QT_DEBUG
                keys.sort();
#endif
                for(const QString &key: keys){
                    save(s, node.toDocumentType().notations().namedItem(key), 0, indent);
                }

                s << QLatin1Char(']');
            }
            s << QLatin1Char('>') << Qt::endl;
        }

    }else if(node.isElement()){
        if(!node.previousSibling().isText()){
            s << QString(indent < 1 ? 0 : depth*indent, QLatin1Char(' '));
        }

        //for avoid duplicate
        QHash<QString, QString> tag_ns_hash;

        //open
        s << QLatin1Char('<') << node.nodeName();
        if(!node.namespaceURI().isEmpty()){
            s << QStringLiteral(" xmlns");
            if(!node.prefix().isEmpty()){
                s << QLatin1Char(':') << node.prefix();
            }
            s << QStringLiteral("=\"") << encodeAttributeValue(node.namespaceURI()) << QStringLiteral("\"");
            //for avoid duplicate
            tag_ns_hash[node.namespaceURI()] = node.prefix();
        }

        //attributes
        if(node.hasAttributes()){
            QStringList attr_names;
            QHash<QString, QDomNode> attr_hash;
            for(int i=0; i<node.attributes().count(); i++){
                attr_hash[node.attributes().item(i).nodeName()] = node.attributes().item(i);
            }
            attr_names = attr_hash.keys();
#ifdef QT_DEBUG
            attr_names.sort();
#endif
            for(const QString &attr_name: attr_names){
                if(namespaceProcessing && !attr_hash[attr_name].namespaceURI().isEmpty()){
                    save(s, node.attributes().namedItemNS(attr_hash[attr_name].namespaceURI(), attr_hash[attr_name].localName()), 0, indent, tag_ns_hash);
                    tag_ns_hash[attr_hash[attr_name].namespaceURI()] = attr_hash[attr_name].prefix();
                }else{
                    save(s, node.attributes().namedItem(attr_hash[attr_name].localName()), 0, indent, tag_ns_hash);
                }
            }
        }

        //children and close
        if(!node.hasChildNodes()){
            s << QStringLiteral("/>");
        }else{
            s << QLatin1Char('>');
            if(!node.childNodes().at(0).isText()){
                if(indent != -1){
                    s << Qt::endl;
                }
            }
            //children
            for(int i=0; i < node.childNodes().length(); i++){
                save(s, node.childNodes().at(i), depth + 1, indent);
            }
            //close
            if(!node.childNodes().at(node.childNodes().length()-1).isText()){
                s << QString(indent < 1 ? 0 : depth*indent, QLatin1Char(' '));
            }
            s << QStringLiteral("</") << node.nodeName() << QLatin1Char('>');
        }

        if(!node.nextSibling().isText()){
            if(indent != -1){
                s << Qt::endl;
            }
        }

    }else if(node.isEntity()){
        node.toEntity().save(s, indent);

    }else if(node.isEntityReference()){
        node.toEntityReference().save(s, indent);

    }else if(node.isNotation()){
        node.toNotation().save(s, indent);

    }else if(node.isNull()){

    }else if(node.isProcessingInstruction()){
        node.toProcessingInstruction().save(s, indent);

    }else if(node.isText()){
        node.toText().save(s, indent);

    }else {
        //qDebug() << QStringLiteral("unknown") << node.nodeType();
    }
}

QString QDomDocumentCompat::encodeAttributeValue(const QString &text) const
{
    QString ret;
    QString key;
    for(int i=0; i<text.length(); i++){
        if(text.at(i) == QLatin1Char('&')){
            ret += QStringLiteral("&amp;");

        }else if(text.at(i) == QLatin1Char('<')){
            ret += QStringLiteral("&lt;");

        }else if(text.at(i) == QLatin1Char('>') && i >= 2 && text.at(i-1) == QLatin1Char(']') && text.at(i-2) == QLatin1Char(']')){
            ret += QStringLiteral("&gt;");

        }else if(text.at(i) == QLatin1Char('\"')){
            ret += QStringLiteral("&quot;");

// Unnecessary encode single quotes , because attribute use always double quote.
//        }else if(text.at(i) == QLatin1Char('\'')){
//            ret += QStringLiteral("&apos;");

        }else if(text.at(i) == QChar(0x09)){
            ret += QStringLiteral("&#x9;");
        }else if(text.at(i) == QChar(0x0d)){
            ret += QStringLiteral("&#xd;");
        }else if(text.at(i) == QChar(0x0a)){
            ret += QStringLiteral("&#xa;");

        }else{
            ret += text.at(i);
        }
    }
    return ret;
}

QXmlSimpleHandler::QXmlSimpleHandler(QDomDocument *doc, bool namespaceProcessing)
    : QXmlDefaultHandler()
    , document(doc)
    , namespaceProcessing(namespaceProcessing)
    , in_cdata(false)
{
    Q_ASSERT(doc);
}

void QXmlSimpleHandler::setDocumentLocator(QXmlLocator *locator)
{
    QXmlDefaultHandler::setDocumentLocator(locator);
}

bool QXmlSimpleHandler::startDocument()
{
    //qDebug() << "startDocument";
    return true;
}

bool QXmlSimpleHandler::endDocument()
{
//    qDebug() << "endDocument";
    return currentNode.isDocument();
}

bool QXmlSimpleHandler::startPrefixMapping(const QString &prefix, const QString &uri)
{
    //qDebug() << "startPrefixMapping" << prefix << uri;
    return QXmlDefaultHandler::startPrefixMapping(prefix, uri);
}

bool QXmlSimpleHandler::endPrefixMapping(const QString &prefix)
{
    //qDebug() << "endPrefixMapping" << prefix;
    return QXmlDefaultHandler::endPrefixMapping(prefix);
}

bool QXmlSimpleHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    //qDebug() << "startElement" << namespaceURI << localName << qName;
    QDomElement element;

    if(namespaceProcessing){
        element = document->createElementNS(namespaceURI, qName);
    }else{
        element = document->createElement(qName);
    }

    for(int i=0; i<atts.length(); i++){
        //qDebug() << atts.uri(i) << atts.qName(i) << atts.value(i);
        if(namespaceProcessing){
            element.setAttributeNS(atts.uri(i), atts.qName(i), atts.value(i));
        }else{
            element.setAttribute(atts.qName(i), atts.value(i));
        }
    }

    if(currentNode.isNull()){
        document->appendChild(element);
    }else{
        currentNode.appendChild(element);
    }

    currentNode = element;

    return true;
}

bool QXmlSimpleHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    Q_UNUSED(localName)
    //qDebug() << "endElement" << namespaceURI << localName << qName;

    if((currentNode.namespaceURI() != namespaceURI)
            || (currentNode.nodeName() != qName)){
        m_errorString = QStringLiteral("Tag missmatch...Start:%1, End:%2")
                .arg(currentNode.toElement().tagName())
                .arg(qName);
        return false;
    }else{
        currentNode = currentNode.parentNode();
        return true;
    }
}

bool QXmlSimpleHandler::characters(const QString &ch)
{
    //qDebug() << "characters" << ch;
    if(in_cdata){
        QDomCDATASection cdata = document->createCDATASection(ch);
        currentNode.appendChild(cdata);
    }else{
        currentNode.appendChild(document->createTextNode(ch));
    }
    return true;
}

bool QXmlSimpleHandler::ignorableWhitespace(const QString &ch)
{
    //qDebug() << "ignorableWhitespace" << ch;
    return QXmlDefaultHandler::ignorableWhitespace(ch);
}

bool QXmlSimpleHandler::processingInstruction(const QString &target, const QString &data)
{
    //qDebug() << "processingInstruction" << target << data;
    QDomNode n = document->createProcessingInstruction(target, data);
    if(currentNode.isNull()){
        document->appendChild(n);
    }else{
        currentNode.appendChild(n);
    }
    return true;
}

bool QXmlSimpleHandler::skippedEntity(const QString &name)
{
    //qDebug() << "skippedEntity" << name;
    currentNode.appendChild(document->createEntityReference(name));
    return true;
}

QString QXmlSimpleHandler::errorString() const
{
    return m_errorString;
}

bool QXmlSimpleHandler::startDTD(const QString &name, const QString &publicId, const QString &systemId)
{
    //qDebug() << "startDTD" << name << publicId << systemId;
    QDomImplementation impl;
    QDomDocumentType type = impl.createDocumentType(name, publicId, systemId);
    QDomDocument doc(type);
    for(int i=0; i<document->childNodes().length(); i++){
        doc.appendChild(document->childNodes().at(i).cloneNode(true));
    }
    *document = doc;
    return true;
}

bool QXmlSimpleHandler::endDTD()
{
    //qDebug() << "endDTD";
    return true;
}

bool QXmlSimpleHandler::startEntity(const QString &name)
{
    Q_UNUSED(name)
    //qDebug() << "startEntity" << name;
    m_errorString = QStringLiteral("Not support: \"reader.setFeature(QStringLiteral(\"http)://qt-project.org/xml/features/report-start-end-entity\"), true);\"");
    return false;
}

bool QXmlSimpleHandler::endEntity(const QString &name)
{
    Q_UNUSED(name)
    //qDebug() << "endEntity" << name;
    return true;
}

bool QXmlSimpleHandler::startCDATA()
{
    in_cdata = true;
    return true;
}

bool QXmlSimpleHandler::endCDATA()
{
    in_cdata = false;
    return true;
}

bool QXmlSimpleHandler::comment(const QString &ch)
{
    //qDebug() << "comment" << ch;
    currentNode.appendChild(document->createComment(ch));
    return true;
}

bool QXmlSimpleHandler::notationDecl(const QString &name, const QString &publicId, const QString &systemId)
{
    //qDebug() << "notationDecl" << name << publicId << systemId;
    return QXmlDefaultHandler::notationDecl(name, publicId, systemId);
}

bool QXmlSimpleHandler::unparsedEntityDecl(const QString &name, const QString &publicId, const QString &systemId, const QString &notationName)
{
    //qDebug() << "unparsedEntityDecl" << name << publicId << systemId << notationName;
    return QXmlDefaultHandler::unparsedEntityDecl(name, publicId, systemId, notationName);
}

bool QXmlSimpleHandler::attributeDecl(const QString &eName, const QString &aName, const QString &type, const QString &valueDefault, const QString &value)
{
    //qDebug() << "attributeDecl" << eName << aName << type << valueDefault << value;
    return QXmlDefaultHandler::attributeDecl(eName, aName, type, valueDefault, value);
}

bool QXmlSimpleHandler::internalEntityDecl(const QString &name, const QString &value)
{
    //qDebug() << "internalEntityDecl" << name << value;
    return QXmlDefaultHandler::internalEntityDecl(name, value);
}

bool QXmlSimpleHandler::externalEntityDecl(const QString &name, const QString &publicId, const QString &systemId)
{
    //qDebug() << "externalEntityDecl" << name << publicId << systemId;
    return QXmlDefaultHandler::externalEntityDecl(name, publicId, systemId);
}

bool QXmlSimpleHandler::warning(const QXmlParseException &exception)
{
    qDebug() << "warning" << exception.message();
    return true;
}

bool QXmlSimpleHandler::error(const QXmlParseException &exception)
{
    qDebug() << "error" << exception.message();
    return true;
}

bool QXmlSimpleHandler::fatalError(const QXmlParseException &exception)
{
    //qDebug() << "fatalError" << exception.message() << exception.lineNumber() << exception.columnNumber();
    m_errorInfo.message = exception.message();
    m_errorInfo.lineNumber = exception.lineNumber();
    m_errorInfo.columnNumber = exception.columnNumber();
    return QXmlDefaultHandler::fatalError(exception);
}

const ErrorInfo &QXmlSimpleHandler::errorInfo() const
{
    return m_errorInfo;
}



