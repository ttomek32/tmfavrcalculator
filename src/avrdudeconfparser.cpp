#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QRegExp>
#include "avrdudeconfparser.h"

const QChar AvrdudeConfParser::CommentChar = '#';
const QChar AvrdudeConfParser::CloseTag = ';';

const QString AvrdudeConfParser::PartOpenTag = "part";
const QString AvrdudeConfParser::ProgrammerOpenTag = "programmer";
const QString AvrdudeConfParser::MemoryOpenTag = "memory";
const QStringList AvrdudeConfParser::DeviceOpeningTags = QStringList()
        << PartOpenTag
        << ProgrammerOpenTag;

// Initializes new instance of AvrDudeConfParser object.
// param QString confFilePath - path to the avrdude.conf file with its name and extension.
AvrdudeConfParser::AvrdudeConfParser(QString confFilePath)
{
    m_Programmers = QVector<Programmer>(0);
    m_Parts = QVector<Part>(0);

    QFile file(confFilePath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        Parse(&file);
    else
    {
        /// TODO: something went really bad
    }
}

// Gets all programmers specified in config file.
QVector<Programmer> AvrdudeConfParser::GetProgrammers()
{
    return m_Programmers;
}

// Gets all parts specified in config file.
QVector<Part> AvrdudeConfParser::GetParts()
{
    return m_Parts;
}

Part AvrdudeConfParser::GetPartByDescription (QString Desc)
{
    QVector<Part> Parts=GetParts();
    for(int i=0; i<Parts.size(); i++)
    {
        if(Desc.compare(Parts[i].GetDescription(), Qt::CaseInsensitive)==0) return Parts[i];

    }
    return Part();  //Return empty Part object - given Desc has not been found
}

Part AvrdudeConfParser::GetPartBySignature(QString sig)
{
    QVector<Part> Parts=GetParts();
    for(int i=0; i<Parts.size(); i++)
    {
        if(sig.compare(Parts[i].GetSignature(), Qt::CaseInsensitive)==0) return Parts[i];

    }
    return Part();  //Return empty Part object - given Desc has not been found
}

void AvrdudeConfParser::Parse(QFile *file)
{
    QTextStream in(file);
    QVector<QString> section(0);
    bool isSectionOpened = false;
    bool isMemoryAtrributeOpened = false;

    while (!in.atEnd())
    {
        QString line = in.readLine().simplified();
        bool skipLine = line.size() == 0
                || line.at(0) == CommentChar;

        if(skipLine)
            continue;

        if(DeviceOpeningTags.contains(line))
            isSectionOpened = true;
        else if (line.startsWith(MemoryOpenTag, Qt::CaseSensitive))
            isMemoryAtrributeOpened = true;
        else if (line == CloseTag)
        {
            if(isMemoryAtrributeOpened)
                isMemoryAtrributeOpened = false;
            else if(isSectionOpened)
            {
                isSectionOpened = false;
                ProcessProperties(&section);
            }
        }

        if(isSectionOpened)
            section.append(line);
    }
}

void AvrdudeConfParser::ProcessProperties(QVector<QString> *section)
{
    const QRegExp Regex("[\";]");
    const QString AssignFilter = " = ";
    const int KeyValueLen = 2;

    QString sectionName = section->at(0);
    QMap<QString, QString> map;


    for(int i = 1; i < section->size(); i++)
    {
        QString line = section->at(i);
        line.replace(Regex, "");

        QStringList params = line.split(AssignFilter);

        if(params.size() == KeyValueLen)
            map[params[KeyValueLen - 2]] = params[KeyValueLen - 1];
    }

    if(sectionName == ProgrammerOpenTag)
        MapProgrammer(map);
    else if(sectionName == PartOpenTag)
        MapPart(map);

    section->clear();
}

void AvrdudeConfParser::MapProgrammer(QMap<QString, QString> map)
{
    Programmer programmer = Programmer(
                map["id"],
                map["desc"],
                map["type"]);

    m_Programmers.append(programmer);
}

void AvrdudeConfParser::MapPart(QMap<QString, QString> map)
{
    Part part = Part(
                map["id"],
                map["desc"],
                map["signature"],
                StringToBool(map["has_jtag"]),
                StringToBool(map["has_debugwire"]),
                StringToBool(map["has_pdi"]),
                StringToBool(map["has_tpi"]));
    m_Parts.append(part);
}

bool AvrdudeConfParser::StringToBool(QString string)
{
    const QString yes = "yes";
    return string.compare(yes, string, Qt::CaseInsensitive) == 0;
}
