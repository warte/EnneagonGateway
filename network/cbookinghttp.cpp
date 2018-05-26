#include "cbookinghttp.h"
#include "manage/cmanage.h"
CBookingHttp::CBookingHttp(QObject *parent) : QObject(parent)
{

}

void CBookingHttp::init()
{
    m_manager = new QNetworkAccessManager(this);
    m_bRet = false;
    m_bRequesting = false;
    CConfigDataHelper data;
}

/*
 * 全部流水号都从新平台的地址上获取
*/
void CBookingHttp::request(const QByteArray& data)
{
    if(m_bRequesting == false)
    {
        m_bRequesting = true;
    }
    else
    {
        return;
    }

    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QUrl url;
    url.setUrl(CManage::getInstance()->getRegUrl() + "equipment/charger/validate_orderedcharger/jx");

    QNetworkRequest req;
    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");

    m_reply = m_manager->post(req, data);

    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    m_timer->singleShot(10000, this, SLOT(reqTimeout()));
}

void CBookingHttp::reqTimeout()
{
    if(m_bRet == true)
    {
        m_reply->close();
        m_bRet = false;
    }
}

void CBookingHttp::pushTimeout()
{

}

void CBookingHttp::reqReply()
{
    m_bRequesting = false;
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    m_bRet = true;
    QByteArray data = m_reply->readAll();
    bool bok;
    QtJson::JsonObject res = QtJson::parse(data,bok).toMap();
    QtJson::JsonObject dataRes = res["data"].toMap();
    int status = res["status"].toInt();
    if(data.size() == 0)
    {
        m_reply->close();
        return;
    }

    qDebug() << "platform register reply:" << data;
    m_reply->close();

    if((status == 200) | (status == 502))
    {
        // get json
        QString enterpriseCode = dataRes["operatorId"].toString();

        QString chargerNo = dataRes["chargerNo"].toString();
        QString enterpriseId = dataRes["enterpriseId"].toString();
        QString stationId = dataRes["stationId"].toString();



        QString type = dataRes["type"].toString();
        QString chargeSn = dataRes["chargerSn"].toString();

        CManage::getInstance()->setEnterpriseCode(enterpriseCode);
        CManage::getInstance()->setEnterpriseID(enterpriseId.toInt());
        CManage::getInstance()->setSiteCode(stationId);
        CManage::getInstance()->getPileActivateMsg(chargeSn, true);
    }

    emit sendRetMsg(data);
}
