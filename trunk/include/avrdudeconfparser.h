#ifndef AVRDUDECONFPARSER_H
#define AVRDUDECONFPARSER_H

#include <QVector>
#include <QString>
#include <QFile>

#include "part.h"
#include "programmer.h"

class AvrdudeConfParser
{
public:
    AvrdudeConfParser(QString confFilePath);

    QVector<Programmer> GetProgrammers();
    QVector<Part> GetParts();
private:
    void Parse(QFile *file);
    void ProcessProperties(QVector<QString> *section);

    QVector<Programmer> m_Programmers;
    QVector<Part> m_Parts;
};

#endif // AVRDUDECONFPARSER_H
