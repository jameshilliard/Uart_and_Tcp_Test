#include "StdAfx.h"
#include "MySerial.h"


MySerial::MySerial(void)
{
}

int MySerial::Open(MyUart *com)
{
	hCom = CreateFile(com->port_name,//COM��
		GENERIC_READ|GENERIC_WRITE, //�������д
		0, //��ռ��ʽ
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���
		0, //ͬ����ʽ
		NULL);

	if( hCom == (HANDLE)-1 )
	{
		return -1;
	}

	COMMTIMEOUTS timeout;
	memset(&timeout,0,sizeof(COMMTIMEOUTS));
	timeout.ReadIntervalTimeout = 10;
	timeout.ReadTotalTimeoutMultiplier=1;
	timeout.ReadTotalTimeoutConstant=1;
	timeout.WriteTotalTimeoutMultiplier=1;
	timeout.WriteTotalTimeoutConstant=1;
	SetCommTimeouts(hCom, &timeout);
	SetCommMask(hCom, EV_RXCHAR);// �Ѽ����ʱ��Ϊ��󣬰��ܳ�ʱ��Ϊ0������ReadFile�������ز���ɲ���

	DCB dcb;
	memset(&dcb,0,sizeof(dcb));
	char buf[512] ={0};

	if(com->dcb.Parity==0)
		com->dcb.Parity = 78;
	else if(com->dcb.Parity==1)
		com->dcb.Parity = 79; 
	else if(com->dcb.Parity==2)
		com->dcb.Parity = 69; 

	sprintf(buf,"%d,%c,%d,%d",com->dcb.BaudRate, com->dcb.Parity, com->dcb.ByteSize, com->dcb.StopBits);

	if( !BuildCommDCB(buf,&dcb) )
		return -1;

	if( !SetCommState(hCom,&dcb) )
		return -1;

	SetupComm(hCom, 2048, 2048);//���û�����
	PurgeComm(hCom, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);

	return 0;
}

int	MySerial::Read(void *buffer,int len)
{
	BOOL bReadStat;
	DWORD r_len = 0;
	
	ClearCommError(hCom,&dwErrorFlags,&ComStat);

	while( ComStat.cbInQue == 0 )
	{
		ClearCommError(hCom,&dwErrorFlags,&ComStat);
	}

	bReadStat = ReadFile(hCom,buffer,len,&r_len,NULL);

	PurgeComm(hCom, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);

	if(!bReadStat)
		return -1;

	return r_len;
}
int MySerial::Write(void *buffer,int len)
{
	BOOL bWriteStat;
	DWORD  w_len = 0;
	
	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	
	bWriteStat = WriteFile(hCom,buffer,len,&w_len,NULL);

	if( !bWriteStat )
	{
		return -1;
	}

	return w_len;
}
void MySerial::ClearErrors()
{
	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	PurgeComm(hCom,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);//��д֮ǰҪ��ջ�����
}

int MySerial::Close()
{
	if( NULL != hCom )
		CloseHandle(hCom);

	return 0;
}

MySerial::~MySerial(void)
{
}
