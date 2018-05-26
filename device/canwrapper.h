#ifndef CANWRAPPER_H
#define CANWRAPPER_H

#include <QString>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>

class CanWrapper
{
public:
    CanWrapper();

    bool Init(const char *interfaceName, int &errorCode);

    void Close();

    bool SendMsg(struct can_frame msg, bool extended, bool rtr, int &errorCode);

    bool GetMsg(struct can_frame &frame, bool &extended, bool &rtr, bool &error, int &errorCode, struct timeval timeout);

    bool SetRecvBufferSize(int size);

    void EnableErrorMessages();

private:
    bool m_initialized;

    int m_socket;


};

#endif // CANWRAPPER_H
