#ifndef PART_H
#define PART_H

#include <QString>

#include "Bit.h"

class Part
{
public:
    // Initializes new instance of Part object.
    Part();

    // Initializes new instance of Part object.
    // param QString id - unique ID number of the part.
    // param QString description - description of the part.
    // param QString signature - the parts signature.
    // param bool hasJTAG - determines whether the part is programmable via JTAG.
    // param bool hasDebugwire - determines whether the part is programmable via Debug Wire.
    // param bool hasPDI - determines whether the part is programmable via PDI.
    // param bool hasTPI - determines whether the part is programmable via TPI.
    Part(
            QString id,
            QString description,
            QString signature,
            bool hasJTAG,
            bool hasDebugwire,
            bool hasPDI,
            bool hasTPI);

    // Gets the parts ID.
    QString GetID();

    // Gets the parts description.
    QString GetDescription();

    // Gets the parts signature.
    QString GetSignature();

    // Gets the lock bits.
    QVector<Bit> GetLockBits();

    // Gets the fuse bits.
    QVector<Bit> GetFuseBits();

    // Sets the lock bits.
    void SetLockBits(QVector<Bit> lockbits);

    // Sets the fuse bits.
    void SetFuseBits(QVector<Bit> fusebits);

    // Determines whether part is programmable via JTAG.
    bool HasJTAG();

    // Determines whether part is programmable via Debug Wire.
    bool HasDebugwire();

    // Determines whether part is programmable via PDI.
    bool HasPDI();

    // Determines whether part is programmable via TPI.
    bool HasTPI();

private:
    QString m_ID;
    QString m_Description;
    QString m_Signature;
    bool m_HasJTAG;
    bool m_HasDebugWire;
    bool m_HasPDI;
    bool m_HasTPI;

    QVector<Bit> m_LockBits;
    QVector<Bit> m_FuseBits;
};

#endif // PART_H
