#include "cdatauploader.h"
#include "manage/capp.h"
#include "manage/cmanage.h"

CDataUploader* CDataUploader::mInstance = NULL;

CDataUploader::CDataUploader(QObject *parent) : QObject(parent)
{
}

void CDataUploader::init()
{
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(checkTimeout()));
    mTimer->start(30 * 1000);
}

// 打开本地数据库
CDataUploader* CDataUploader::getInstance()
{
    if(!mInstance)
    {
        mInstance = new CDataUploader();
        //mInstance->init();
    }

    return mInstance;
}


void CDataUploader::checkTimeout()
{
    searchForRecordForUploaded();
}

void CDataUploader::searchForRecordForUploaded()
{
    CPileDataHelper::getInstance()->queryOrderList();
}

void CDataUploader::getDataForUploaded(QSqlRecord rec)
{    
    QVariantMap param;
    QString connectorCode = rec.value(rec.indexOf("connectorcode")).toString();
    QString flowNo = rec.value(rec.indexOf("flowno")).toString();
    param.insert("orderNo", rec.value(rec.indexOf("flowno")).toString());
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("equipmentCode", connectorCode.left(connectorCode.length() - 1));
    param.insert("operatorCode", CApp::getInstance()->getSEnterpriseCode());
    param.insert("connectorCode", connectorCode);
    param.insert("userChargeType", rec.value(rec.indexOf("userChargeType")).toString());
    param.insert("vin", rec.value(rec.indexOf("vin")).toString());
    param.insert("startTime", rec.value(rec.indexOf("starttime")).toString());
    param.insert("startType", rec.value(rec.indexOf("starttype")).toString());
    param.insert("endTime", rec.value(rec.indexOf("endtime")).toString());
    param.insert("startSoc", rec.value(rec.indexOf("startsoc")).toInt());
    param.insert("endSoc", rec.value(rec.indexOf("endsoc")).toInt());
    param.insert("chargerDuration", rec.value(rec.indexOf("chargetime")).toInt());
    param.insert("stopType", rec.value(rec.indexOf("stoptype")).toString());
    param.insert("paymentAmount",  rec.value(rec.indexOf("amount")).toFloat());
    param.insert("paymentAccount", rec.value(rec.indexOf("paymentAccount")).toString());
    param.insert("payChannel", "");
    param.insert("physicsCardNo", rec.value(rec.indexOf("cardno")).toString());
    param.insert("money", rec.value(rec.indexOf("amount")).toFloat());
    param.insert("electMoney", rec.value(rec.indexOf("amount")).toFloat());
    float sharp = rec.value(rec.indexOf("sharp")).toFloat();
    float peak = rec.value(rec.indexOf("peak")).toFloat();
    float flat = rec.value(rec.indexOf("flat")).toFloat();
    float valley = rec.value(rec.indexOf("valley")).toFloat();
    float quantity = rec.value(rec.indexOf("quantity")).toFloat();
    float servicemoney = CApp::getInstance()->getSharpService() * sharp + CApp::getInstance()->getPeakService() * peak + CApp::getInstance()->getFlatService() * flat + CApp::getInstance()->getValleyService() * valley;
    param.insert("serviceMoney", QString::number(servicemoney,'f', 2));
    param.insert("elect", QString::number(quantity, 'f', 2));
    param.insert("cuspElect", QString::number(sharp, 'f', 2));
    param.insert("cuspElectPrice", QString::number(CApp::getInstance()->getSharpRate(), 'f', 2));
    param.insert("cuspServicePrice", QString::number(CApp::getInstance()->getSharpService(), 'f', 2));
    param.insert("cuspMoney", QString::number(CApp::getInstance()->getSharpTotal() * sharp , 'f', 2));
    param.insert("cuspElectMoney", QString::number(CApp::getInstance()->getSharpRate() * sharp, 'f', 2));
    param.insert("cuspServiceMoney", QString::number(CApp::getInstance()->getSharpService() * sharp, 'f', 2));
    param.insert("peakElect", QString::number(peak, 'f', 2));
    param.insert("peakElectPrice", QString::number(CApp::getInstance()->getPeakRate(), 'f', 2));
    param.insert("peakServicePrice", QString::number(CApp::getInstance()->getPeakService(), 'f', 2));
    param.insert("peakMoney", QString::number(CApp::getInstance()->getpeakTotal() * peak, 'f', 2));
    param.insert("peakElectMoney", QString::number(peak * CApp::getInstance()->getPeakRate(), 'f', 2));
    param.insert("peakServiceMoney", QString::number(peak * CApp::getInstance()->getPeakService(), 'f', 2));
    param.insert("flatElect", QString::number(flat, 'f', 2));
    param.insert("flatElectPrice", QString::number(CApp::getInstance()->getFlatRate(), 'f', 2));
    param.insert("flatServicePrice", QString::number(CApp::getInstance()->getFlatService(), 'f', 2));
    param.insert("flatMoney", QString::number(flat * CApp::getInstance()->getFlatTotal(), 'f', 2));
    param.insert("flatElectMoney", QString::number(flat * CApp::getInstance()->getFlatRate(), 'f', 2));
    param.insert("flatServiceMoney", QString::number(flat * CApp::getInstance()->getFlatService(), 'f', 2));
    param.insert("valleyElect", QString::number( valley, 'f', 2));
    param.insert("valleyElectPrice", QString::number(CApp::getInstance()->getValleyRate(), 'f', 2));
    param.insert("valleyServicePrice", QString::number(CApp::getInstance()->getValleyService(), 'f', 2));
    param.insert("valleyMoney", QString::number(valley * CApp::getInstance()->getValleyTotal(), 'f', 2));
    param.insert("valleyElectMoney", QString::number(valley * CApp::getInstance()->getValleyRate(), 'f', 2));
    param.insert("valleyServiceMoney", QString::number( valley * CApp::getInstance()->getValleyService(), 'f', 2));
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QByteArray data = QtJson::serialize(param);
    qDebug() << "send database record msg" << data;


    AESHandler handler;
    CManage::getInstance()->sendRecordMsgToNetwork(connectorCode, flowNo, handler.encodeMsg(data));
}
