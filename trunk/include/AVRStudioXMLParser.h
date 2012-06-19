#ifndef AVRSTUDIOXMLPARSER_H
#define AVRSTUDIOXMLPARSER_H

#include <QFile>
#include <QString>
#include <QDomDocument>

#include "Bit.h"
#include "Value.h"
#include "Part.h"

class AVRStudioXMLParser
{
public:
    // Initializes new instance of Part object.
    // param QString configDirPath - path to the directory that contains xml configuration files from the avrstudio.
    // param Part* pPart - pointer to the Part to which lockbits and fusebits should be applied.
    AVRStudioXMLParser(QString configDirPath, Part *pPart);
private:
    QString GetXQuery();
    void Parse(QString configDirPath, Part *pPart);
    QVector<Bit> GetBits(QDomNode node);
    QVector<Value> GetValues(QDomNodeList nodelist);
    QVector<ProgrammingInterface> GetInterfaces(QDomNode node);
};

#endif // AVRSTUDIOXMLPARSER_H
