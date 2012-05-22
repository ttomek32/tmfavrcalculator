#ifndef PART_H
#define PART_H

#include <QString>

class Part
{
public:
    Part();
    Part(
            QString id,
            QString description,
            QString signature,
            bool hasJTAG,
            bool hasDebugwire,
            bool hasPDI,
            bool hasTPI);

    QString GetID();
    QString GetDescription();
    QString GetSignature();
    bool HasJTAG();
    bool HasDebugwire();
    bool HasPDI();
    bool HasTPI();

private:
    QString m_ID;
    QString m_Description;
    QString m_Signature;
    bool m_HasJTAG;
    bool m_HasDebugWire;
    bool m_HasPDI;
    bool m_HasTPI;
};

#endif // PART_H
