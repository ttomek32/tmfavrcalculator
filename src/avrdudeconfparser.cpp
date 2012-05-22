#include <QtCore>
#include <QFile>
#include "avrdudeconfparser.h"

static const QChar CommentChar = '#';

AvrdudeConfParser::AvrdudeConfParser(QString confFilePath)
{
    m_Programmers = QVector<Programmer>();
    m_Parts = QVector<Part>();

    QFile file(confFilePath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        Parse(&file);
    else
    {
        /// TODO: something went really bad
    }
}

QVector<Programmer> AvrdudeConfParser::GetProgrammers()
{
    return m_Programmers;
}

QVector<Part> AvrdudeConfParser::GetParts()
{
    return m_Parts;
}

void AvrdudeConfParser::Parse(QFile *file)
{
    QTextStream in(file);
    QVector<QString> section(0);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        bool skipLine = line.size() == 0
                || line.at(0) == CommentChar;

        if(skipLine)
            continue;

        section.append(line);
       // if(line.at(0) == ';')
            ProcessProperties(&section);
    }
}

void AvrdudeConfParser::ProcessProperties(QVector<QString> *section)
{
    section->clear();
}
