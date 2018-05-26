#include "cstatehttp.h"
#include "manage/cmanage.h"
#include "manage/capp.h"
#include "pile/cconnectormanager.h"

CStateHttp::CStateHttp(QObject *parent) : QObject(parent)
{
    init();
}

void CStateHttp::init()
{
    m_manager = new QNetworkAccessManager(this);
    m_bRet = false;
    m_bRequesting = false;
}

/*
 * 全部流水号都从新平台的地址上获取
*/
void CStateHttp::request(const QByteArray& data)
{
    if(m_bRequesting == false)
    {
        m_bRequesting = true;
    }
    else
    {
        return;
    }

    QUrl url;
    url.setUrl(CApp::getInstance()->getSBaseUrl() + "changePortState");

    QNetworkRequest req;
    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

    m_reply = m_manager->post(req, data);

    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    m_timer->singleShot(5000, this, SLOT(reqTimeout()));
}

void CStateHttp::reqTimeout()
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


void CStateHttp::reqReply()
{
    m_bRequesting = false;
    m_bRet = true;
    QByteArray tdata = m_reply->readAll();

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


    QtJson::JsonObject res = QtJson::parse(data,bok).toMap();



    QString code = res["connectorCode"].toString();

    m_reply->close();

    deleteLater();
}
