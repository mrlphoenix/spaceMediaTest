#include <QUrl>

#include "sslencoder.h"
#include "globalconfig.h"
#include "singleton.h"

SSLEncoder::SSLEncoder(QObject *parent) : QObject(parent)
{

}

QByteArray SSLEncoder::encodeAES(QByteArray data, QString password)
{
    int outLen = 0;
    QByteArray dataBuff;dataBuff.resize(data.size()+AES_BLOCK_SIZE);
    //        encryptProcess.start("openssl enc -aes-256-cbc -base64 -K " + hexSessionKey + " -iv 30303030303030303030303030303030");
    EVP_CIPHER_CTX evpCipherCtx;
    EVP_CIPHER_CTX_init(&evpCipherCtx);
    EVP_EncryptInit(&evpCipherCtx, EVP_aes_256_cbc(),
                    (const unsigned char*)password.toLocal8Bit().data(),
                    (const unsigned char*)QString("0000000000000000").toLocal8Bit().toHex().data());
    EVP_EncryptUpdate(&evpCipherCtx, (unsigned char*)dataBuff.data(), &outLen, (const unsigned char*)data.data(), data.size());
    int tempLen = outLen;
    EVP_EncryptFinal(&evpCipherCtx, (unsigned char*)dataBuff.data() + tempLen, &outLen);
    tempLen+=outLen;
    EVP_CIPHER_CTX_cleanup(&evpCipherCtx);
    dataBuff.resize(tempLen);
    return dataBuff;
}

QByteArray SSLEncoder::decodeAES(QByteArray data, QString password)
{
    int outLen=0;
    QByteArray dataBuff;dataBuff.resize(data.size()+AES_BLOCK_SIZE);
    EVP_CIPHER_CTX evpCipherCtx;
    EVP_CIPHER_CTX_init(&evpCipherCtx);
    EVP_DecryptInit(&evpCipherCtx,EVP_aes_256_cbc(),
                    (const unsigned char*)password.toLocal8Bit().data(),
                    (const unsigned char*)QString("0000000000000000").toLocal8Bit().data());
    EVP_DecryptUpdate(&evpCipherCtx,(unsigned char*)dataBuff.data(),&outLen,(const unsigned char*)data.data(),data.size());
    int tempLen=outLen;
    EVP_DecryptFinal(&evpCipherCtx,(unsigned char*)dataBuff.data()+tempLen,&outLen);
    tempLen+=outLen;
    EVP_CIPHER_CTX_cleanup(&evpCipherCtx);
    dataBuff.resize(tempLen);
    return dataBuff;
}

QByteArray SSLEncoder::sha256(const QByteArray &text)
{
    unsigned int outLen=0;
    QByteArray dataBuff; dataBuff.resize(EVP_MAX_MD_SIZE);
    EVP_MD_CTX evpMdCtx;
    EVP_DigestInit(&evpMdCtx, EVP_sha256());
    EVP_DigestUpdate(&evpMdCtx, text.data(), text.size());
    EVP_DigestFinal_ex(&evpMdCtx, (unsigned char *)dataBuff.data(), &outLen);
    EVP_MD_CTX_cleanup(&evpMdCtx);
    dataBuff.resize(outLen);
    return dataBuff.toHex();
}

QString SSLEncoder::encodeBase64Data(QByteArray data)
{

    QByteArray result = encodeAES(data,GlobalConfigInstance.getSessionKey().toLocal8Bit().toHex());
    QString base64 = result.toBase64();

    QString base64UrlEncoded = QUrl::toPercentEncoding(base64.replace("\n","").replace("\r",""));
    return base64UrlEncoded;

}

QByteArray SSLEncoder::decodeFromBase64Data(QByteArray base64Data)
{
    QByteArray encryptedData = QByteArray::fromBase64(base64Data);
    QByteArray decodedData = decodeAES(encryptedData, GlobalConfigInstance.getSessionKey().toLocal8Bit().toHex());
    return decodedData;
}
