#include "lfsrencoder.h"

#include <QCryptographicHash>
#include <QDataStream>

void LFSR::Encoder::encode(QByteArray &data, QString pass)
{
    QByteArray hashArray = QCryptographicHash::hash(pass.toLocal8Bit(),QCryptographicHash::Md5);
    hashArray = hashArray.mid(0,4);
    QDataStream ds(hashArray);

    int seed;
    ds >> seed;

    for (int i = 0; i<data.count(); ++i)
    {
        char currentByte = 0;
        for (int bitIndex = 0; bitIndex < 8; ++bitIndex)
        {
            if (seed & 0x00000001)
            {
                seed = (seed ^ 0x80000057 >> 1) | 0x80000000;
                currentByte |= (1 << bitIndex);
            }
            else
                seed >>= 1;
        }
        data[i] = data[i] ^ currentByte;
    }
}
