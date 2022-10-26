#ifndef _ICANCmd_H_
#define _ICANCmd_H_

#include <windows.h>

//�ӿڿ����Ͷ���
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

//�������÷���״ֵ̬
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
    DWORD Mode;        // ����ģʽ
    DWORD LocalPort;   // ���ض˿�
    DWORD RemotePort;  // �豸�˿�
    DWORD RemoteIp[2]; // �豸IP��֧��IP��
}EthArgs_t;

//CAN������
enum CAN_ErrorCode
{
    CAN_E_NOERROR               = 0x0000,   // û�з��ִ���
    CAN_E_OVERFLOW              = 0x0001,   // CAN�������ڲ�FIFO���
    CAN_E_ERRORALARM            = 0x0002,   // CAN���������󱨾�
    CAN_E_PASSIVE               = 0x0004,   // CAN��������������
    CAN_E_LOSE                  = 0x0008,   // CAN�������ٲö�ʧ
    CAN_E_BUSERROR              = 0x0010,   // CAN���������ߴ���

    CAN_E_DEVICEOPENED          = 0x0100,       // �豸�Ѿ���
    CAN_E_DEVICEOPEN            = 0x0200,       // ���豸����
    CAN_E_DEVICENOTOPEN         = 0x0400,       // �豸û�д�
    CAN_E_BUFFEROVERFLOW        = 0x0800,       // ���������
    CAN_E_DEVICENOTEXIST        = 0x1000,       // ���豸������
    CAN_E_LOADKERNELDLL         = 0x2000,       // װ�ض�̬��ʧ��
    CAN_E_CMDFAILED             = 0x4000,       // ִ������ʧ�ܴ�����
    CAN_E_BUFFERCREATE          = 0x8000,       // �ڴ治��
};

enum ParamType
{
    PARAM_EVENT_RCV_WND  = 0x8000,           // �����¼����ھ��
    PARAM_EVENT_RCV_WMID = 0x8001,                   // �����¼���ϢID
};
//CAN����֡����
typedef  struct  tagCAN_DataFrame {
    UINT	uTimeFlag;                      // ʱ���ʶ,�Խ���֡��Ч
    BYTE	nSendType;                      // ����֡����,0-��������;1-���η���;2-�Է�����;3-�����Է�����
    BYTE	bRemoteFlag;                    // �Ƿ���Զ��֡
    BYTE	bExternFlag;                    // �Ƿ�����չ֡
    BYTE	nDataLen;                       // ���ݳ���
    UINT	uID;                            // ����DI
    BYTE	arryData[8];                    // ��������
#ifdef _DLLRCVTIME
    UINT    uDllRcvTimeFlag;                                // for debug
#endif
}CAN_DataFrame, *PCAN_DataFrame;

//CAN��ʼ������
typedef struct tagCAN_InitConfig {
    UCHAR	bMode;                          // ����ģʽ(0��ʾ����ģʽ,1��ʾֻ��ģʽ)
    BYTE	nBtrType;                       // λ��ʱ����ģʽ(1��ʾSJA1000,0��ʾLPC21XX)
    BYTE    dwBtr[4];                                               // CANλ��ʱ����
    DWORD	dwAccCode;                                              // ������
    DWORD	dwAccMask;                                              // ������
    BYTE	nFilter;                                                // �˲���ʽ(0��ʾδ�����˲�����,1��ʾ˫�˲�,2��ʾ���˲�)
    BYTE    dwReserved;                     // Ԥ���ֶ�
}CAN_InitConfig,*PCAN_InitConfig;

//CAN�豸��Ϣ
typedef  struct  tagCAN_DeviceInformation {
    USHORT      uHardWareVersion;               // Ӳ���汾
    USHORT      uFirmWareVersion;               // �̼��汾
    USHORT      uDriverVersion;                 // �����汾
    USHORT      uInterfaceVersion;              // �ӿڿ�汾
    USHORT      uInterruptNumber;               // �жϺ�
    BYTE        bChannelNumber;                 // �м�·CAN
    CHAR        szSerialNumber[20];             // �豸���к�
    CHAR        szHardWareType[40];             // Ӳ������
    CHAR        szDescription[20];                          // �豸����
}CAN_DeviceInformation, *PCAN_DeviceInformation;

//CAN������Ϣ
typedef struct tagCAN_ErrorInformation {
    UINT    uErrorCode;             // ��������
    BYTE    PassiveErrData[3];      // ������������
    BYTE    ArLostErrData;          // �ٲô�������
}CAN_ErrorInformation, *PCAN_ErrorInformation;

// ���豸
DWORD __stdcall CAN_DeviceOpen(DWORD dwType, DWORD dwIndex, CHAR *pDescription);
// �ر��豸
DWORD __stdcall CAN_DeviceClose(DWORD dwDeviceHandle);

// ����CAN
DWORD __stdcall CAN_ChannelStart(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_InitConfig pInitConfig);
// ֹͣCAN
DWORD __stdcall CAN_ChannelStop(DWORD dwDeviceHandle, DWORD dwChannel);

// ��ȡ�豸��Ϣ
DWORD __stdcall CAN_GetDeviceInfo(DWORD dwDeviceHandle, PCAN_DeviceInformation pInfo);
// ��ȡCAN������Ϣ
DWORD __stdcall CAN_GetErrorInfo(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_ErrorInformation pErr);

// ��EEPROM
DWORD __stdcall CAN_ReadEEPROM(DWORD dwDeviceHandle, WORD dwAddr, PBYTE pBuff, WORD nLen);
// дEEPROM
DWORD __stdcall CAN_WriteEEPROM(DWORD dwDeviceHandle, WORD dwAddr, PBYTE pBuff, WORD nLen);

// ��������
ULONG __stdcall CAN_ChannelSend(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_DataFrame pSend, ULONG nCount);
// �ӽ��ջ������ж�����
ULONG __stdcall CAN_ChannelReceive(DWORD dwDeviceHandle, DWORD dwChannel, PCAN_DataFrame pReceive, ULONG nCount, INT nWaitTime = -1);
// ��ȡ���ջ�����֡��
ULONG __stdcall CAN_GetReceiveCount(DWORD dwDeviceHandle, DWORD dwChannel);
// ��ս��ջ�����
DWORD __stdcall CAN_ClearReceiveBuffer(DWORD dwDeviceHandle, DWORD dwChannel);

//���Ĵ���
DWORD __stdcall CAN_ReadRegister(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwAddr, PBYTE pBuff, WORD nLen);
//д�Ĵ���
DWORD __stdcall CAN_WriteRegister(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwAddr, PBYTE pBuff, WORD nLen);

// ��ȡ����
DWORD __stdcall CAN_GetParam(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwParamType, PVOID pData);
// ���ò���
DWORD __stdcall CAN_SetParam(DWORD dwDeviceHandle, DWORD dwChannel, DWORD dwParamType, PVOID pData);


#endif //_ICANCmd_H_
