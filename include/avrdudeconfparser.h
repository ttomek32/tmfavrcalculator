#ifndef AVRDUDECONFPARSER_H
#define AVRDUDECONFPARSER_H

#include <QVector>
#include <QString>
#include <QMap>
#include <QFile>

#include "part.h"
#include "programmer.h"

class AvrdudeConfParser
{
    static const QChar CommentChar;
    static const QChar CloseTag;
    static const QString PartOpenTag;
    static const QString ProgrammerOpenTag;
    static const QString MemoryOpenTag;
    static const QStringList DeviceOpeningTags;

public:
    // Initializes new instance of AvrDudeConfParser object.
    // param QString confFilePath - path to the avrdude.conf file with its name and extension.
    AvrdudeConfParser(QString confFilePath);

    // Gets all programmers specified in config file.
    QVector<Programmer> GetProgrammers();

    // Gets all parts specified in config file.
    QVector<Part> GetParts();
private:
    void Parse(QFile *file);
    void ProcessProperties(QVector<QString> *section);
    void MapProgrammer(QMap<QString, QString> map);
    void MapPart(QMap<QString, QString> map);

    bool StringToBool(QString string);

    QVector<Programmer> m_Programmers;
    QVector<Part> m_Parts;
};

#endif // AVRDUDECONFPARSER_H
