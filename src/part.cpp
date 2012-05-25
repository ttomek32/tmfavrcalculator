#include "part.h"

// Initializes new instance of Part object.
Part::Part() { }

// Initializes new instance of Part object.
// param QString id - unique ID number of the part.
// param QString description - description of the part.
// param QString signature - the parts signature.
// param bool hasJTAG - determines whether the part is programmable via JTAG.
// param bool hasDebugwire - determines whether the part is programmable via Debug Wire.
// param bool hasPDI - determines whether the part is programmable via PDI.
// param bool hasTPI - determines whether the part is programmable via TPI.
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


    m_Signature = signature.remove(" 0x"); //Combine signature bytes to form 24-bit hex signature
    m_HasJTAG = hasJTAG;
    m_HasDebugWire = hasDebugwire;
    m_HasPDI = hasPDI;
    m_HasTPI = hasTPI;
}

// Gets the parts ID.
QString Part::GetID()
{
    return m_ID;
}

// Gets the parts description.
QString Part::GetDescription()
{
    return m_Description;
}

// Gets the parts signature.
QString Part::GetSignature()
{
    return m_Signature;
}

// Determines whether part is programmable via JTAG.
bool Part::HasJTAG()
{
    return m_HasJTAG;
}

// Determines whether part is programmable via Debug Wire.
bool Part::HasDebugwire()
{
    return m_HasDebugWire;
}

// Determines whether part is programmable via PDI.
bool Part::HasPDI()
{
    return m_HasPDI;
}

// Determines whether part is programmable via TPI.
bool Part::HasTPI()
{
    return m_HasTPI;
}
