#include "canthread.h"
#include <QDebug>

CanThread::CanThread()
{
    user_thread = new CanUser();
    stop_flag = false;
}

void CanThread::run()
{
    while(!stop_flag)
    {
        if(status == RECIEVE)
        {
            user_thread->ReceiveData();
            user_thread->GetClusterStatus();
            user_thread->GetClusterGeneral();
        }
        else if(status == SEND)
            qDebug() << "send..." << count++;

        QThread::msleep(200);
    }
    stop_flag = false;
}

void CanThread::stop()
{
    stop_flag = true;
}

void CanThread::setStatus(uint8_t flag)
{
    status = flag;
}
