#ifndef _ICANCmd_H_
#define _ICANCmd_H_

#include <windows.h>

//接口卡类型定义
#define ACUSB_131B              1
#define ACUSB_132B              2
#define ACPCI_251               3
#define ACPCI_252               4
#define ACPCI_254               5

#define ACNET_600               6
#define ACNET_622               7

#define LCPCIE_251              8
#define LCPCIE_252              9

#define LCPCIE_361              10
#define LCPCIE_362              11
#define LCPCIE_364              12

#define LCUSB_131B              1
#define LCUSB_132B              2
#define LCMiniPcie_431          1
#define LCMiniPcie_432          2
#define LCPCI_252               4

#define USBCAN_1CH              13
#define USBCAN_C_1CH            14
#define USBCAN_E_1CH            15
#define USBCAN_E_2CH            16
#define MPCIeCAN_1CH            17
#define MPCIeCAN_2CH            18

//函数调用返回状态值
#define	CAN_RESULT_OK           1
#define CAN_RESULT_ERROR        0

#define  ARGS_ADD_IP       1
#define  ARGS_DEL_IP       2
#define  ARGS_INIT_IP      0
#define  ARGS_IP_NUM       3
#define  ARGS_IP_GET       4


typedef struct
{
    DWORD MagicNum;    // = 0xAA55BBCC
    DWORD Mode;        // 工作模式
    DWORD LocalPort;   // 本地端口
    DWORD RemotePort;  // 设备端口
    DWORD RemoteIp[2]; // 设备IP，支持IP段
}EthArgs_t;

//CAN错误码
enum CAN_ErrorCode
{
    CAN_E_NOERROR               = 0x0000,   // 没有发现错误
    CAN_E_OVERFLOW              = 0x0001,   // CAN控制器内部FIFO溢出
    CAN_E_ERRORALARM            = 0x0002,   // CAN控制器错误报警
    CAN_E_PASSIVE               = 0x0004,   // CAN控制器消极错误
    CAN_E_LOSE                  = 0x0008,   // CAN控制器仲裁丢失
    CAN_E_BUSERROR              = 0x0010,   // CAN控制器总线错误

    CAN_E_DEVICEOPENED          = 0x0100,       // 设备已经打开
    CAN_E_DEVICEOPEN            = 0x0200,       // 打开设备错误
    CAN_E_DEVICENOTOPEN         = 0x0400,       // 设备没有打开
    CAN_E_BUFFEROVERFLOW        = 0x0800,       // 缓冲区溢出
    CAN_E_DEVICENOTEXIST        = 0x1000,       // 此设备不存在
    CAN_E_LOADKERNELDLL         = 0x2000,       // 装载动态库失败
    CAN_E_CMDFAILED             = 0x4000,       // 执行命令失败错误码
    CAN_E_BUFFERCREATE          = 0x8000,       // 内存不足
};

enum ParamType
{
    PARAM_EVENT_RCV_WND  = 0x8000,           // 接收事件窗口句柄
    PARAM_EVENT_RCV_WMID = 0x8001,                   // 接收事件消息ID
};
//CAN数据帧类型
typedef  struct  tagCAN_DataFrame {
    UINT	uTimeFlag;                      // 时间标识,对接收帧有效
    BYTE	nSendType;                      // 发送帧类型,0-正常发送;1-单次发送;2-自发自收;3-单次自发自收
    BYTE	bRemoteFlag;                    // 是否是远程帧
    BYTE	bExternFlag;                    // 是否是扩展帧
    BYTE	nDataLen;                       // 数据长度
    UINT	uID;                            // 报文DI
    BYTE	arryData[8];                    // 报文数据
#ifdef _DLLRCVTIME
    UINT    uDllRcvTimeFlag;                                // for debug
#endif
}CAN_DataFrame, *PCAN_DataFrame;

//CAN初始化配置
typedef struct tagCAN_InitConfig {
    UCHAR	bMode;                          // 工作模式(0表示正常模式,1表示只听模式)
    BYTE	nBtrType;                       // 位定时参数模式(1表示SJA1000,0表示LPC21XX)
    BYTE    dwBtr[4];                                               // CAN位定时参数
    DWORD	dwAccCode;                                              // 验收码
    DWORD	dwAccMask;                                              // 屏蔽码
    BYTE	nFilter;                                                // 滤波方式(0表示未设置滤波功能,1表示双滤波,2表示单滤波)
    BYTE    dwReserved;                     // 预留字段
}CAN_InitConfig,*PCAN_InitConfig;

//CAN设备信息
typedef  struct  tagCAN_DeviceInformation {
    USHORT      uHardWareVersion;               // 硬件版本
    USHORT      uFirmWareVersion;               // 固件版本
    USHORT      uDriverVersion;                 // 驱动版本
    USHORT      uInterfaceVersion;              // 接口库版本
    USHORT      uInterruptNumber;               // 中断号
    BYTE        bChannelNumber;                 // 有几路CAN
    CHAR        szSerialNumber[20];             // 设备序列号
    CHAR        szHardWareType[40];             // 硬件类型
    CHAR        szDescription[20];                          // 设备描述
}CAN_DeviceInformation, *PCAN_DeviceInformation;

//CAN错误信息
typedef struct tagCAN_ErrorInformation {
    UINT    uErrorCode;             // 错误类型
    BYTE    PassiveErrData[3];      // 消极错误数据
    BYTE    ArLostErrData;          // 仲裁错误数据
}CAN_ErrorInformation, *PCAN_ErrorInformation;

// 打开设备
DWORD __stdcall CAN_DeviceOpen(DWORD dwType, DWORD dwIndex, CHAR *pDescription);
// 关闭设备
DWORD __stdcall CAN_DeviceClose(DWORD dwDeviceHandle);

// 启动CAN
DWORD __stdcall CAN_ChannelStart(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_InitConfig pInitConfig);
// 停止CAN
DWORD __stdcall CAN_ChannelStop(DWORD dwDeviceHandle, DWORD dwChannel);

// 获取设备信息
DWORD __stdcall CAN_GetDeviceInfo(DWORD dwDeviceHandle, PCAN_DeviceInformation pInfo);
// 获取CAN错误信息
DWORD __stdcall CAN_GetErrorInfo(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_ErrorInformation pErr);

// 读EEPROM
DWORD __stdcall CAN_ReadEEPROM(DWORD dwDeviceHandle, WORD dwAddr, PBYTE pBuff, WORD nLen);
// 写EEPROM
DWORD __stdcall CAN_WriteEEPROM(DWORD dwDeviceHandle, WORD dwAddr, PBYTE pBuff, WORD nLen);

// 发送数据
ULONG __stdcall CAN_ChannelSend(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_DataFrame pSend, ULONG nCount);
// 从接收缓冲区中读数据
ULONG __stdcall CAN_ChannelReceive(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_DataFrame pReceive, ULONG nCount, INT nWaitTime = -1);
// 获取接收缓冲区帧数
ULONG __stdcall CAN_GetReceiveCount(DWORD dwDeviceHandle, DWORD dwChannel);
// 清空接收缓冲区
DWORD __stdcall CAN_ClearReceiveBuffer(DWORD dwDeviceHandle, DWORD dwChannel);

//读寄存器
DWORD __stdcall CAN_ReadRegister(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwAddr, PBYTE pBuff, WORD nLen);
//写寄存器
DWORD __stdcall CAN_WriteRegister(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwAddr, PBYTE pBuff, WORD nLen);

// 获取参数
DWORD __stdcall CAN_GetParam(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwParamType, PVOID pData);
// 设置参数
DWORD __stdcall CAN_SetParam(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwParamType, PVOID pData);


#endif //_ICANCmd_H_
