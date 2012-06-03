#ifndef AVRFACTORY_H
#define AVRFACTORY_H

#include "AVRDudeConfParser.h"
#include "AVRStudioXMLParser.h"

class AVRFactory
{
public:
    // Initializes new instance of Part object.
    // param QString avrDudeConfPath - path to the avrdude config file. It should contain it's name and extension.
    // param QString avrStudioConfDirPath - path to the directory that contains xml configuration files from the avrstudio.
    AVRFactory(QString avrDudeConfPath, QString avrStudioConfDirPath);

    // Gets all programmers.
    QVector<Programmer> GetProgrammers();

    // Gets all parts.
    QVector<Part> GetParts();

    //Returns Part record with Description field equal to Desc
    Part GetPartByDescription (QString Desc);

    //Returns Part record with Signature field equal to sig
    Part GetPartBySignature (QString sig);

private:
    QVector<Programmer> m_Programmers;
    QVector<Part> m_Parts;
};

#endif // AVRFACTORY_H
