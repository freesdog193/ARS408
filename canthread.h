#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include "canuser.h"

#define RECIEVE 0
#define SEND    1

class CanThread : public QThread
{
public:
    CanThread();
    void stop();
    void setStatus(uint8_t flag);

    CanUser *user_thread;
protected:
    void run();
private:
    bool stop_flag;
    uint16_t count;
    uint8_t status;
};

#endif // CANTHREAD_H
