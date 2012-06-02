#include "Bit.h"

QMap<QString, QString*> Bit::GetMappingMap()
{
    QMap<QString, QString*> map;

    map["caption"] = &m_Caption;
    map["name"] = &m_Name;
    map["mask"] = &m_Mask;
    map["offset"] = &m_Offset;

    return map;
}

void Bit::SetValues(QVector<Value> values)
{
    m_Values = values;
}
