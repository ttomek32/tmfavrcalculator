#include "Value.h"

QMap<QString, QString*> Value::GetMappingMap()
{
    QMap<QString, QString*> map;

    map["caption"] = &m_Caption;
    map["name"] = &m_Name;
    map["value"] = &m_Value;

    return map;
}

QString Value::GetName()
{
    return m_Name;
}

QString Value::GetCaption()
{
    return m_Caption;
}

QString Value::GetValue()
{
    return m_Value;
}
