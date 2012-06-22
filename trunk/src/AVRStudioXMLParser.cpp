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
    configDirPath.append(pPart->GetDescription().toLower())
                 .append(".xml");
    Parse(configDirPath, pPart);
}

void AVRStudioXMLParser::Parse(QString configDirPath, Part *pPart)
{
    QXmlQuery query;
    QString result;
    query.bindVariable("partName", QVariant(configDirPath));
    query.setQuery(GetXQuery());
    query.evaluateTo(&result);

    // for future use
    QString errorMsg;
    int line;
    int col;

    QDomDocument doc;
    if(!doc.setContent(result, &errorMsg, &line, &col))
        return;

    QDomNode root = doc.firstChild();
    QDomNode fuses = root.firstChild();
    QDomNode locks = fuses.nextSibling();
    QDomNode interfaces = locks.nextSibling();

    pPart->SetFuseBits(GetBits(fuses.firstChild()));
    pPart->SetLockBits(GetBits(locks.firstChild()));
    pPart->SetProgrammingInterfaces(GetInterfaces(interfaces.firstChild()));
}

QVector<Bit> AVRStudioXMLParser::GetBits(QDomNode node)
{
    QVector<Bit> bits(0);

    while(!node.isNull())
    {
        QDomNamedNodeMap attributes = node.attributes();
        Bit bit;

        for(int i = 0; i < attributes.size(); i++)
        {
            QDomNode attribute = attributes.item(i);
            if(bit.GetMappingMap()[attribute.nodeName()])
                *bit.GetMappingMap()[attribute.nodeName()] = attribute.nodeValue();
        }

        bit.SetValues(GetValues(node.childNodes()));
        bits.append(bit);

        node = node.nextSibling();
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

QVector<ProgrammingInterface> AVRStudioXMLParser::GetInterfaces(QDomNode node)
{
    QVector<ProgrammingInterface> interfaces(0);

    while(!node.isNull())
    {
        QDomNamedNodeMap attributes = node.attributes();
        ProgrammingInterface interface;

        for(int i = 0; i < attributes.size(); i++)
        {
            QDomNode attribute = attributes.item(i);
            if(interface.GetMappingMap()[attribute.nodeName()])
                *interface.GetMappingMap()[attribute.nodeName()] = attribute.nodeValue();
        }

        interfaces.append(interface);
        node = node.nextSibling();
    }

    return interfaces;
}

QString AVRStudioXMLParser::GetXQuery()
{
    QFile file(":/resources/xquery/AVRStudioFilter.xq");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    return in.readAll();
}
