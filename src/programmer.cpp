#include "programmer.h"

Programmer::Programmer() { }

Programmer::Programmer(QString id, QString description, QString type)
{
    m_ID = id;
    m_Description = description;
    m_Type = type;
}

QString Programmer::GetID()
{
    return m_ID;
}

QString Programmer::GetDescription()
{
    return m_Description;
}

QString Programmer::GetType()
{
    return m_Type;
}
