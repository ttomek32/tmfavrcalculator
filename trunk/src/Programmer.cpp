#include "Programmer.h"

// Initializes new instance of Programmer object.
Programmer::Programmer() { }

// Initializes new instance of Programmer object.
// param QString id - unique ID number of the programmer.
// param QString description - description of the programmer.
// param QString type - type of the programmer.
Programmer::Programmer(QString id, QString description, QString type)
{
    m_ID = id;
    m_Description = description;
    m_Type = type;
}

// Gets the programmers ID.
QString Programmer::GetID()
{
    return m_ID;
}

// Gets the programmers description.
QString Programmer::GetDescription()
{
    return m_Description;
}

// Gets the type of the programmer.
QString Programmer::GetType()
{
    return m_Type;
}
