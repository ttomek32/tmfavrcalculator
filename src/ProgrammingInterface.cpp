#include "ProgrammingInterface.h"

QMap<QString, QString*> ProgrammingInterface::GetMappingMap()
{
    QMap<QString, QString*> map;
    map["name"] = &m_Name;
    map["type"] = &m_Type;

    return map;
}
