#include "Part.h"

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

// Gets the lock bits.
QVector<Bit> Part::GetLockBits()
{
    return m_LockBits;
}

// Gets the fuse bits.
QVector<Bit> Part::GetFuseBits()
{
    return m_FuseBits;
}

// Gets the programming interfaces.
QVector<ProgrammingInterface> Part::GetProgrammingInterfaces()
{
    return m_Interfaces;
}

// Sets the lock bits.
void Part::SetLockBits(QVector<Bit> lockbits)
{
    m_LockBits = lockbits;
}

// Sets the fuse bits.
void Part::SetFuseBits(QVector<Bit> fusebits)
{
    m_FuseBits = fusebits;
}

// Sets the programming interfaces.
void Part::SetProgrammingInterfaces(QVector<ProgrammingInterface> interfaces)
{
    m_Interfaces = interfaces;
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

QMap<int, QString> Part::GetFusesNames()
{
    QMap<int, QString> fuses;
    for(int i = 0; i < m_FuseBits.size(); i++)
    {
        Bit fusebit = m_FuseBits.at(i);
        volatile unsigned int offset = fusebit.GetOffset().remove("0x").toUInt();

        fuses[offset] = fusebit.GetRegisterName();
    }

    return fuses;
}



















