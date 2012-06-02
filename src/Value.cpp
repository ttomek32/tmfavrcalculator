#include "Value.h"

Value::Value()
{

}

QMap<QString, QString*> Value::GetMappingMap()
{
    QMap<QString, QString*> map;

    map["caption"] = &m_Caption;
    map["name"] = &m_Name;
    map["value"] = &m_Value;

    return map;
}
