#include "creghttp.h"
#include "manage/capp.h"
#include "manage/cmanage.h"
#include "pile/cconnectormanager.h"

CRegHttp::CRegHttp(QObject *parent) : QObject(parent)
{
    init();
}

void CRegHttp::init()
{
    m_manager = new QNetworkAccessManager(this);
    m_bRet = false;
    m_bRequesting = false;
}

/*
 * 全部流水号都从新平台的地址上获取
*/
void CRegHttp::request(const QByteArray& data)
{
    QUrl url;
    url.setUrl(CApp::getInstance()->getSBaseUrl() + "equipmentReg");

    QNetworkRequest req;
    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");

    m_reply = m_manager->post(req, data);

    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    m_timer->singleShot(10000, this, SLOT(reqTimeout()));
}

void CRegHttp::reqTimeout()
{
    if(m_bRet == true)
    {
        m_reply->close();
        m_bRet = false;
    }
    else
    {
        deleteLater();
    }
}


void CRegHttp::reqReply()
{
    m_bRequesting = false;
    m_bRet = true;
    QByteArray tdata = m_reply->readAll();

    qDebug() << "reg reply " << tdata;

    m_reply->close();
    if(tdata.size() == 0)
    {
        deleteLater();
        return;
    }

    AESHandler handler;

    int status;
    bool bok;

    QByteArray data = handler.decodeMsg(tdata, status);

    // get json
    QtJson::JsonObject dataRes = QtJson::parse(data, bok).toMap();

    QString sn = dataRes["equipmentCode"].toString();

    if((status == 200))
    {
        QString mqHost = dataRes["mqHost"].toString();
        QString mqPort = dataRes["mqPort"].toString();
        QString mqUser = dataRes["mqUser"].toString();
        QString mqPwd = dataRes["mqPwd"].toString();


        CManage::getInstance()->updateMQConnectionInfo(mqHost, mqPort, mqUser, mqPwd);

        CApp::getInstance()->setSMqUrl(mqHost);
        CApp::getInstance()->setSMqPort(mqPort);
        CApp::getInstance()->setSMqUser(mqUser);
        CApp::getInstance()->setSMqPassword(mqPwd);

        //CManage::getInstance()->getPileActivateMsg(chargeSn, true);
        CConnectorManager::getInstance()->getRegisteActivateMsg(sn, true);
    }
    else
    {
        CConnectorManager::getInstance()->getRegisteActivateMsg(sn, false);
    }

    deleteLater();
}
