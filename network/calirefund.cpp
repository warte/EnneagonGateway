#include "calirefund.h"
#include "pile/cconnectormanager.h"
#include <unistd.h>
#include "manage/cmanage.h"

CAliRefund::CAliRefund(QObject *parent) : QObject(parent)
{
    init();
}

void CAliRefund::init()
{
    m_manager = new QNetworkAccessManager(this);

    m_flowRet = false;
}

/*
 * 全部流水号都从新平台的地址上获取
 * connectorID: 桩号 + 枪号
 * data: 请求的数据，包括具体的桩号信息以及vin
*/
void CAliRefund::request(const QByteArray& data)
{
    m_flowRet = false;

    mData = data;

    QUrl url;
    url.setUrl(CApp::getInstance()->getSBaseUrl() + "aliRefund");
    QNetworkRequest req;
    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    m_reply = m_manager->post(req, data);
    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    QTimer::singleShot(5000, this, SLOT(reqTimeout()));
}

void CAliRefund::reqTimeout()
{
    if(m_flowRet == false)
    {
        m_reply->close();

        // 退款超时，继续退款，直到成功为止
        CManage::getInstance()->sendAliRefund(mData);
        deleteLater();
    }
    else
    {
        deleteLater();

        m_flowRet = false;
    }
}

void CAliRefund::reqReply()
{
    m_flowRet = true;

    QByteArray tdata = m_reply->readAll();
    m_reply->close();

    if(tdata.size() == 0)
    {
        CConnectorManager::getInstance()->getPlatformFlowRetMsg(m_connectorID, false);
        deleteLater();
        return;
    }

    AESHandler handler;

    int status;
    bool bok;

    QByteArray data = handler.decodeMsg(tdata, status);

    // get json
    QtJson::JsonObject dataRes = QtJson::parse(data, bok).toMap();


    if(status != 200){
        // 继续提交退款请求
        CManage::getInstance()->sendAliRefund(mData);
    }

    deleteLater();
}
