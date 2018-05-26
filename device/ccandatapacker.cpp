#include "ccandatapacker.h"
#include "pile/cconnectormanager.h"

void CCANDataPakcer::setDevID(QString &connectorID)
{
    uDevID = CConnectorManager::getInstance()->getDevIDByConnetorID(connectorID);
}

void CCANDataPakcer::setDevID(ushort devID)
{
    uDevID = devID;
}
