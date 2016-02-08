#ifndef SSLENCODER_H
#define SSLENCODER_H

#include <QObject>
#include <QByteArray>
#include <openssl/aes.h>
#include <openssl/evp.h>

class SSLEncoder : public QObject
{
    Q_OBJECT
public:
    explicit SSLEncoder(QObject *parent = 0);
    QString encodeBase64Data(QByteArray data);
    QByteArray decodeFromBase64Data(QByteArray base64Data);

signals:

public slots:
protected:

    QByteArray encodeAES(QByteArray data, QString password);
    QByteArray decodeAES(QByteArray data, QString password);
    QByteArray sha256(const QByteArray &text);
};

#endif // SSLENCODER_H
