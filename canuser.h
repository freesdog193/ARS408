#ifndef CANUSER_H
#define CANUSER_H

#include "ICANCmd.h"
#include <QLibrary>
#include <QVector>

typedef DWORD(__stdcall CanDeviceOpen)(DWORD, DWORD, CHAR*);
typedef DWORD(__stdcall CanGetDeviceInfo)(DWORD, PCAN_DeviceInformation);
typedef DWORD(__stdcall CanChannelStart)(DWORD, DWORD, PCAN_InitConfig);
typedef ULONG(__stdcall CanChannelRecieve)(DWORD, DWORD, PCAN_DataFrame, ULONG, INT);
typedef DWORD(__stdcall CanGetErrorInfo)(DWORD, DWORD, PCAN_ErrorInformation);
typedef DWORD(__stdcall CanChannelStop)(DWORD, DWORD);
typedef DWORD(__stdcall CanDeviceClose(DWORD));

struct Cluster_status{
    uint8_t near_num;
    uint8_t far_num;
    int meas_counter;
};

struct Cluster_general
{
    uint8_t ID;
    float X;
    float Y;
    float Vx;
    float Vy;
    float RCS;
};


class CanUser
{
public:
    CanUser();
    CanUser(int _CanDevIndex, int _CanChannel, int _CanSendType, int _CanSendFrames,
            int _CanSendTimes, int _CanSendDelay);
    bool OpenDevice();
    void CanChannelConfig();
    void ChannelStart();
    QVector<QString> GetDeviceInfo();
    void ReceiveData();
    void GetClusterStatus();
    void GetClusterGeneral();
    void ClusterFilter();
    bool CloseDevice();

    QVector<uint16_t> data;
    QVector<QVector<uint16_t> > data_vector;
    Cluster_status *cluster_status;
    Cluster_general *cluster_general;
    QVector<Cluster_general> cluster_vector;

private:
    int CanDevIndex;
    int CanChannel;
    int CanSendType;
    int CanSendFrames;
    int CanSendTimes;
    int CanSendDelay;

    static DWORD dwDeviceHandle;
    CAN_InitConfig config;
    CAN_DeviceInformation DeviceInfo;
    CanDeviceOpen *pDeviceOpen;
    CanGetDeviceInfo *pGetDeviceInfo;
    CanChannelStart *pChannelStart;
    CanChannelRecieve *pChannelRecieve;
    CanGetErrorInfo *pGetErrorInfo;
    CanDeviceClose *pDeviceClose;
    CanChannelStop *pChannelStop;
};

#endif // CANUSER_H
