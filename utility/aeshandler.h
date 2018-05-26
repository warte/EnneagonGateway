#ifndef AESHANDLER
#define AESHANDLER


#include <crypto/aes.h>
#include <crypto/hex.h>
#include <crypto/base64.h>
#include <crypto/modes.h>
#include <iostream>
#include <sstream>
#include <string>
#include <QDebug>
#include <QByteArray>
#include "utility/json.h"
#include "manage/capp.h"

using namespace std;
using namespace CryptoPP;

class AESHandler
{
public:

    std::string CBC_AESEncryptStr(std::string sKey, std::string sIV,const char *plainText)
    {
        std::string outstr;

        //填key
        SecByteBlock key(AES::MIN_KEYLENGTH);
        memset(key,0x30,key.size() );
        sKey.size()<=AES::MIN_KEYLENGTH?memcpy(key,sKey.c_str(),sKey.size()):memcpy(key,sKey.c_str(),AES::MIN_KEYLENGTH);

        //填iv
        byte iv[AES::BLOCKSIZE];
        memset(iv,0x30,AES::BLOCKSIZE);
    sIV.size()<=AES::BLOCKSIZE?memcpy(iv,sIV.c_str(),sIV.size()):memcpy(iv,sIV.c_str(),AES::BLOCKSIZE);

        AES::Encryption aesEncryption((byte *)key, AES::MIN_KEYLENGTH);

        CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

        StreamTransformationFilter cbcEncryptor(cbcEncryption, new Base64Encoder(new StringSink(outstr)));
        cbcEncryptor.Put((byte *)plainText, strlen(plainText));
        cbcEncryptor.MessageEnd();

        return outstr;
    }

    std::string CBC_AESDecryptStr(std::string sKey, std::string sIV,const char *cipherText)
    {
        std::string outstr;

        //填key
        SecByteBlock key(AES::MIN_KEYLENGTH);
        memset(key,0x30,key.size() );
        sKey.size()<=AES::MIN_KEYLENGTH?memcpy(key,sKey.c_str(),sKey.size()):memcpy(key,sKey.c_str(),AES::MIN_KEYLENGTH);

        //填iv
        byte iv[AES::BLOCKSIZE];
        memset(iv,0x30,AES::BLOCKSIZE);
    sIV.size()<=AES::BLOCKSIZE?memcpy(iv,sIV.c_str(),sIV.size()):memcpy(iv,sIV.c_str(),AES::BLOCKSIZE);


        CBC_Mode<AES >::Decryption cbcDecryption((byte *)key, AES::MIN_KEYLENGTH, iv);

        Base64Decoder decryptor(new StreamTransformationFilter(cbcDecryption, new StringSink(outstr)));
        decryptor.Put((byte*)cipherText, strlen(cipherText));
        decryptor.MessageEnd();

        return outstr;
    }

    QByteArray decodeMsg(const QByteArray &in, int &status)
    {
        bool bok;
        QtJson::JsonObject res = QtJson::parse(in,bok).toMap();

        status = res["status"].toInt();

        std::string sdata = std::string(res["data"].toByteArray().constData(), res["data"].toByteArray().length());

        std::string retstr = CBC_AESDecryptStr(CApp::getInstance()->getKey(), CApp::getInstance()->getIv(), sdata.c_str());

        QByteArray databyte = QByteArray(retstr.c_str(), retstr.length());

        qDebug() << "after decode status:" << status << " data:"<< databyte;

        return databyte;
    }
    QByteArray encodeMsg(const QByteArray &in)
    {
        qDebug() << in;
        std::string sdata = std::string(in.constData(), in.length());

        std::string retstr = CBC_AESEncryptStr(CApp::getInstance()->getKey(), CApp::getInstance()->getIv(), sdata.c_str());

        QByteArray retdata = QByteArray(retstr.c_str(), retstr.length());

        return  retdata.replace(" ", "").replace("\n", "");
    }
};


#endif // AESHANDLER

