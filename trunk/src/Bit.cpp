#include "Bit.h"

QMap<QString, QString*> Bit::GetMappingMap()
{
    QMap<QString, QString*> map;

    map["caption"] = &m_Caption;
    map["name"] = &m_Name;
    map["mask"] = &m_Mask;
    map["offset"] = &m_Offset;
    map["registername"] = &m_RegisterName;

    return map;
}

QString Bit::GetName()
{
    return m_Name;
}

QString Bit::GetCaption()
{
    return m_Caption;
}

QString Bit::GetMask()
{
    return m_Mask;
}

QString Bit::GetOffset()
{
    return m_Offset;
}

QString Bit::GetRegisterName()
{
    return m_RegisterName;
}

QVector<Value> Bit::GetValues()
{
    return m_Values;
}

void Bit::SetValues(QVector<Value> values)
{
    m_Values = values;
}
