#include "part.h"
Part::Part() { }

Part::Part(
        QString id,
        QString description,
        QString signature,
        bool hasJTAG,
        bool hasDebugwire,
        bool hasPDI,
        bool hasTPI)
{
    m_ID = id;
    m_Description = description;
    m_Signature = signature;
    m_HasJTAG = hasJTAG;
    m_HasDebugWire = hasDebugwire;
    m_HasPDI = hasPDI;
    m_HasTPI = hasTPI;
}

QString Part::GetID()
{
    return m_ID;
}

QString Part::GetDescription()
{
    return m_Description;
}

QString Part::GetSignature()
{
    return m_Signature;
}

bool Part::HasJTAG()
{
    return m_HasJTAG;
}

bool Part::HasDebugwire()
{
    return m_HasDebugWire;
}

bool Part::HasPDI()
{
    return m_HasPDI;
}

bool Part::HasTPI()
{
    return m_HasTPI;
}
