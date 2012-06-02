#include <QTextStream>
#include <QXmlQuery>
#include <QVector>

#include "AVRStudioXMLParser.h"
#include "Bit.h"

// Initializes new instance of Part object.
// param QString configDirPath - path to the directory that contains xml configuration files from the avrstudio.
// param Part* pPart - pointer to the Part to which lockbits and fusebits should be applied.
AVRStudioXMLParser::AVRStudioXMLParser(QString configDirPath, Part *pPart)
{
    configDirPath.append(pPart->GetDescription())
                 .append(".xml");

    pPart->SetFuseBits(Parse(configDirPath, FUSE));
    pPart->SetLockBits(Parse(configDirPath, LOCK));
}

QVector<Bit> AVRStudioXMLParser::Parse(QString configDirPath, BitType bitType)
{
    QXmlQuery query;
    QString result;

    query.bindVariable("partName", QVariant(configDirPath));
    query.bindVariable("moduleName", QVariant(BitMapper[bitType]));
    query.setQuery(GetXQuery());
    query.evaluateTo(&result);

    // for future use
    QString errorMsg;
    int line;
    int col;

    QDomDocument doc;
    if(doc.setContent(result, &errorMsg, &line, &col))
       return AnalyseDocument(doc);

    return QVector<Bit>(0);
}

QVector<Bit> AVRStudioXMLParser::AnalyseDocument(QDomDocument doc)
{
    QDomNode root = doc.firstChild();
    QDomNode docNode = root.firstChild();
    QVector<Bit> bits(0);

    while(!docNode.isNull())
    {
        QDomElement e = docNode.toElement();

        if(!e.isNull())
        {
            QDomNamedNodeMap attributes = docNode.attributes();
            Bit bit;

            for(int i = 0; i < attributes.size(); i++)
            {
                QDomNode attribute = attributes.item(i);
                if(bit.GetMappingMap()[attribute.nodeName()])
                    *bit.GetMappingMap()[attribute.nodeName()] = attribute.nodeValue();
            }

            bit.SetValues(GetValues(docNode.childNodes()));
            bits.append(bit);
        }

        docNode = docNode.nextSibling();
    }

    return bits;
}

QVector<Value> AVRStudioXMLParser::GetValues(QDomNodeList nodelist)
{
    QVector<Value> values;

    for(int i = 0; i < nodelist.size(); i++)
    {
        QDomNamedNodeMap attributes =  nodelist.at(i).attributes();
        Value value;

        for(int i = 0; i < attributes.size(); i++)
        {
            QDomNode attribute = attributes.item(i);
            if(value.GetMappingMap()[attribute.nodeName()])
                *value.GetMappingMap()[attribute.nodeName()] = attribute.nodeValue();
        }

        values.append(value);
    }

    return values;
}

QString AVRStudioXMLParser::GetXQuery()
{
    QFile file(":/resources/xquery/AVRStudioFilter.xq");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    return in.readAll();
}
