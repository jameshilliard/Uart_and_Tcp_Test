#pragma once

struct _uart
{
	char port_name[255];
	DCB	 dcb;
};

typedef struct _uart MyUart;

class MySerial
{
public:
	MySerial(void);
	int Open(MyUart *com);
	int	Read(void *buffer,int size);
	int Write(void *buffer,int size);
	void ClearErrors();
	int  Close();
	virtual ~MySerial(void);
private:
	HANDLE	hCom;
	COMSTAT ComStat;
	DWORD	dwErrorFlags;
};

