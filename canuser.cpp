#include "canuser.h"
#include <QDebug>

#define __countof(a) (sizeof(a)/sizeof(a[0]))

DWORD CanUser::dwDeviceHandle = 0;

CanUser::CanUser()
{
    QLibrary lib("CanCmd.dll");
    if(true == lib.load())
    {
        qDebug() << "Loading lib finished.";
        pDeviceOpen = (CanDeviceOpen*)lib.resolve("CAN_DeviceOpen");
        pGetDeviceInfo = (CanGetDeviceInfo*)lib.resolve("CAN_GetDeviceInfo");
        pChannelStart = (CanChannelStart*)lib.resolve("CAN_ChannelStart");
        pChannelRecieve = (CanChannelRecieve*)lib.resolve("CAN_ChannelReceive");
        pGetErrorInfo = (CanGetErrorInfo*)lib.resolve("CAN_GetErrorInfo");
    }
    cluster_status = new Cluster_status();
    cluster_general = new Cluster_general();
}

CanUser::CanUser(int _CanDevIndex, int _CanChannel, int _CanSendType, int _CanSendFrames, int _CanSendTimes, int _CanSendDelay):
    CanDevIndex(_CanDevIndex), CanChannel(_CanChannel), CanSendType(_CanSendType),
    CanSendFrames(_CanSendFrames), CanSendTimes(_CanSendTimes), CanSendDelay(_CanSendDelay)
{
    QLibrary lib("CanCmd.dll");
    if(true == lib.load())
    {
        qDebug() << "Loading lib finished.";
        pDeviceOpen = (CanDeviceOpen*)lib.resolve("CAN_DeviceOpen");
        pGetDeviceInfo = (CanGetDeviceInfo*)lib.resolve("CAN_GetDeviceInfo");
        pChannelStart = (CanChannelStart*)lib.resolve("CAN_ChannelStart");
        pChannelRecieve = (CanChannelRecieve*)lib.resolve("CAN_ChannelReceive");
        pGetErrorInfo = (CanGetErrorInfo*)lib.resolve("CAN_GetErrorInfo");
        pDeviceClose = (CanDeviceClose*)lib.resolve("CAN_DeviceClose");
        pChannelStop = (CanChannelStop*)lib.resolve("CAN_ChannelStop");
    }
}

bool CanUser::OpenDevice()
{
    if((dwDeviceHandle = pDeviceOpen(USBCAN_1CH, CanChannel, 0)) == 0)
    {
        qDebug() << "open device error!";
        return false;
    }
    else
        return true;
}

bool CanUser::CloseDevice()
{
    if(pChannelStop(dwDeviceHandle, CanChannel) == CAN_RESULT_ERROR)
        return false;
    else
    {
        if(pDeviceClose(dwDeviceHandle) == CAN_RESULT_ERROR)
            return false;
        else
            return true;
    }
}

void CanUser::CanChannelConfig()
{
    config.bMode = 0;
    config.dwAccCode = 0;
    config.dwAccMask = 0xffffffff;
    config.nFilter = 0;
    config.nBtrType = 1;
    config.dwBtr[0] = 0x00;
    config.dwBtr[1] = 0x1c;
    config.dwBtr[2] = 0;
    config.dwBtr[3] = 0;
}

void CanUser::ChannelStart()
{
    if(pChannelStart(dwDeviceHandle, CanChannel, &config) != CAN_RESULT_OK)
        qDebug() << "channel start error";
//    else
//        qDebug() << "channel start ok";
}

QVector<QString> CanUser::GetDeviceInfo()
{
    QVector<QString> info;
    if(pGetDeviceInfo(dwDeviceHandle, &DeviceInfo) != CAN_RESULT_OK)
        info.push_back(QString("Get Device Information error!"));
    else
    {
        info.push_back(QString("---%1---").arg(DeviceInfo.szDescription));
        info.push_back(QString("SN:%1").arg(DeviceInfo.szSerialNumber));
        info.push_back(QString("CAN 通道数:%1").arg(DeviceInfo.bChannelNumber));
        info.push_back(QString("硬件  版本:%1").arg(DeviceInfo.uHardWareVersion));
        info.push_back(QString("固件  版本:%1").arg(DeviceInfo.uFirmWareVersion));
        info.push_back(QString("驱动  版本:%1").arg(DeviceInfo.uDriverVersion));
        info.push_back(QString("接口库版本:%1").arg(DeviceInfo.uInterfaceVersion));
//        qDebug() << QString("---%1---").arg(DeviceInfo.szDescription);
//        qDebug() << QString("SN:%1").arg(DeviceInfo.szSerialNumber);
//        qDebug() << QString("CAN 通道数:%1").arg(DeviceInfo.bChannelNumber);
//        qDebug() << QString("硬件  版本:%1").arg(DeviceInfo.uHardWareVersion);
//        qDebug() << QString("固件  版本:%1").arg(DeviceInfo.uFirmWareVersion);
//        qDebug() << QString("驱动  版本:%1").arg(DeviceInfo.uDriverVersion);
//        qDebug() << QString("接口库版本:%1").arg(DeviceInfo.uInterfaceVersion);
    }
    return info;
}

void CanUser::ReceiveData()
{
    CAN_ErrorInformation errInfo;
    CAN_DataFrame buf[100];
    int len;
    bool recieve_flag = false;
    if((len = pChannelRecieve(dwDeviceHandle, 0, buf, __countof(buf), 200)) > 0)
    {
        data_vector.clear();
        for(int i = 0; i < len; i++)
        {
            if(buf[i].uID == 0x0600 && recieve_flag == false)
                recieve_flag = true;
            else if(buf[i].uID == 0x0600 && recieve_flag == true)
                break;

            if(recieve_flag)
            {
                data.push_back(buf[i].uID);
                for(int j = 0; j < 8; j++)
                    data.push_back(buf[i].arryData[j]);
                data_vector.push_back(data);
                data.clear();
            }

        }
//        qDebug() << "over" << data_vector.length();
    }

    if(pGetErrorInfo(dwDeviceHandle, 0, &errInfo) == CAN_RESULT_OK)
    {
        qDebug() << errInfo.uErrorCode;
    }
}

void CanUser::GetClusterStatus()
{
    if(data_vector[0][0] == 0x600)
    {

        cluster_status->near_num = data_vector[0][1];
        cluster_status->far_num = data_vector[0][2];
    }
    else
        qDebug() << "error!";
}

void CanUser::GetClusterGeneral()
{
    cluster_vector.clear();
    for (auto iter = data_vector.begin() + 1; iter != data_vector.end(); ++iter)
    {
        if((*iter)[0] == 0x701)
        {
            cluster_general->ID = (*iter)[1];
            cluster_general->X = ((uint16_t)(*iter)[2] << 5 | (uint16_t)(*iter)[3] >> 3) * 0.2 - 500;
            cluster_general->Y = (((uint16_t)(*iter)[3] << 8 | (uint16_t)(*iter)[4]) & 0x03FF) * 0.2 - 102.3;
            cluster_general->Vx = ((uint16_t)(*iter)[5] << 2 | (uint16_t)(*iter)[6] >> 6) * 0.25 - 128;
            cluster_general->Vy = (((uint16_t)(*iter)[6] << 3 | (uint16_t)(*iter)[7] >> 5) & 0x01FF) * 0.25 - 64;
            cluster_general->RCS = (*iter)[8] * 0.5 - 64.0;

            cluster_vector.push_back(*cluster_general);
        }
//        else
//            qDebug() << "error";

    }
}

void CanUser::ClusterFilter()
{
    for(auto iter = cluster_vector.begin(); iter != cluster_vector.end(); ++iter)
    {
        /*RCS过滤*/
//        if((*iter).RCS < -5)
//            cluster_vector.remove(iter - cluster_vector.begin());


    }


}
