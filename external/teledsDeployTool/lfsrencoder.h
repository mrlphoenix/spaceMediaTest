#ifndef LFSRENCODER_H
#define LFSRENCODER_H

#include <QByteArray>
#include <QString>
namespace LFSR{

class Encoder{
public:
    static void encode(QByteArray &data, QString pass);
};

}

#endif // LFSRENCODER_H
