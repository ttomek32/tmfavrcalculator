#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include <QString>

class Programmer
{
public:
    // Initializes new instance of Programmer object.
    Programmer();

    // Initializes new instance of Programmer object.
    // param QString id - unique ID number of the programmer.
    // param QString description - description of the programmer.
    // param QString type - type of the programmer.
    Programmer(
            QString id,
            QString description,
            QString type);

    // Gets the programmers ID.
    QString GetID();

    // Gets the programmers description.
    QString GetDescription();

    // Gets the type of the programmer.
    QString GetType();
private:
    QString m_ID;
    QString m_Description;
    QString m_Type;
};

#endif // PROGRAMMER_H
