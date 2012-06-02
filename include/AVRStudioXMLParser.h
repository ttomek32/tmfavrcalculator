#ifndef AVRSTUDIOXMLPARSER_H
#define AVRSTUDIOXMLPARSER_H

#include <QFile>
#include <QString>
#include <QDomDocument>
#include <QMap>

#include "Bit.h"
#include "Value.h"
#include "Part.h"

enum BitType
{
    FUSE,
    LOCK
};

static QMap<BitType, QString> InitialiseBitMapper();
static QMap<BitType, QString> BitMapper = InitialiseBitMapper();
static QMap<BitType, QString> InitialiseBitMapper()
{
    QMap<BitType, QString> map;
    map[FUSE] = "FUSE";
    map[LOCK] = "LOCKBIT";

    return map;
}


class AVRStudioXMLParser
{
public:
    // Initializes new instance of Part object.
    // param QString configDirPath - path to the directory that contains xml configuration files from the avrstudio.
    // param Part* pPart - pointer to the Part to which lockbits and fusebits should be applied.
    AVRStudioXMLParser(QString configDirPath, Part *pPart);
private:
    QString GetXQuery();
    QVector<Bit> Parse(QString configDirPath, BitType bitType);
    QVector<Bit> AnalyseDocument(QDomDocument doc);
    QVector<Value> GetValues(QDomNodeList nodelist);
};

#endif // AVRSTUDIOXMLPARSER_H
