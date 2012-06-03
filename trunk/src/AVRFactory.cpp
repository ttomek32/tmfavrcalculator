#include "AVRFactory.h"
#include "AVRDudeConfParser.h"
#include "AVRStudioXMLParser.h"

// Initializes new instance of Part object.
// param QString avrDudeConfPath - path to the avrdude config file. It should contain it's name and extension.
// param QString avrStudioConfDirPath - path to the directory that contains xml configuration files from the avrstudio.
AVRFactory::AVRFactory(QString avrDudeConfPath, QString avrStudioConfDirPath)
{
    AVRDudeConfParser dudeParser(avrDudeConfPath);

    m_Programmers = dudeParser.GetProgrammers();
    m_Parts = dudeParser.GetParts();

    for(int i = 0; i < m_Parts.size(); i++)
        AVRStudioXMLParser(avrStudioConfDirPath, &m_Parts[i]);

    //m_Parts = m_Parts;
}

// Gets all programmers.
QVector<Programmer> AVRFactory::GetProgrammers()
{
    return m_Programmers;
}

// Gets all parts.
QVector<Part> AVRFactory::GetParts()
{
    return m_Parts;
}

Part AVRFactory::GetPartByDescription (QString Desc)
{
    QVector<Part> Parts=GetParts();
    for(int i=0; i<Parts.size(); i++)
    {
        if(Desc.compare(Parts[i].GetDescription(), Qt::CaseInsensitive)==0) return Parts[i];

    }
    return Part();  //Return empty Part object - given Desc has not been found
}

Part AVRFactory::GetPartBySignature(QString sig)
{
    QVector<Part> Parts=GetParts();
    for(int i=0; i<Parts.size(); i++)
    {
        if(sig.compare(Parts[i].GetSignature(), Qt::CaseInsensitive)==0) return Parts[i];

    }
    return Part();  //Return empty Part object - given Desc has not been found
}
