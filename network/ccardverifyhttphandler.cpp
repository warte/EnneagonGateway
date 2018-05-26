#include "ccardverifyhttphandler.h"
#include "device/ccandatapacker.h"
#include "manage/cmanage.h"

CCardVerifyHttpHandler::CCardVerifyHttpHandler(QObject *parent) : QObject(parent)
{
    init();
}

void CCardVerifyHttpHandler::init()
{
    m_manager = new QNetworkAccessManager(this);
    m_bRet = false;
}

/*
 * 全部流水号都从新平台的地址上获取
*/
void CCardVerifyHttpHandler::request(const QByteArray& data, const QString& connectorID, const QString &cardNo)
{
    this->connectorID = connectorID;
    this->cardNo = cardNo;
    QUrl url;
    url.setUrl( CApp::getInstance()->getSBaseUrl() + "verifyCard");

    QNetworkRequest req;
    req.setUrl(url);

    m_bRet = false;

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

    m_reply = m_manager->post(req, data);

    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "send card verify....";

    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));

    m_timer->singleShot(5000, this, SLOT(reqTimeout()));
}

void CCardVerifyHttpHandler::reqTimeout()
{
    if(m_bRet == true)
    {
        m_reply->close();
        m_bRet = false;
    }
    else
    {
        CConnectorManager::getInstance()->getCardVerifyRetMsg(connectorID, 500, cardNo);
        deleteLater();
    }
}

void CCardVerifyHttpHandler::reqReply()
{
    m_bRet = true;
    QByteArray tdata = m_reply->readAll();

    if(tdata.size() == 0)
    {
        CConnectorManager::getInstance()->getCardVerifyRetMsg(connectorID, 500, cardNo);
        deleteLater();
        return;
    }

    m_reply->close();

    AESHandler handler;

    int status;
    bool bok;

    QByteArray data = handler.decodeMsg(tdata, status);

    QtJson::JsonObject dataRes = QtJson::parse(data, bok).toMap();

    QString connectorCode = dataRes["connectorCode"].toString();
    int cardType = dataRes["cardType"].toInt();
    int cardStatus = dataRes["cardStatus"].toInt();
    int balance = dataRes["balance"].toFloat() * 100;

    CCANDataPakcer packer;
    packer.setDevID(CConnectorManager::getInstance()->getDevIDByConnetorID(connectorID));

    CManage::getInstance()->sendMsgToPile(packer.packCardRetMsg(status, balance));

    CConnectorManager::getInstance()->getCardVerifyRetMsg(connectorID, status, cardNo);

    deleteLater();
}
