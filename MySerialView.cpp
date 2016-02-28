#include "StdAfx.h"
#include "Uart and Tcp Test.h"
#include "Uart and Tcp TestDlg.h"
#include "afxdialogex.h"
#include "MySerialView.h"
#include "MySerial.h"
#include "Func.h"

#define COM_CLOSED	0
#define COM_OPEN	1

CEdit		com_rcv_txt;
int			ComState;
int			com_rx_format;
MySerial	myserial;
HANDLE		hrcvThread;

bool		com_timer_msg = false;

CEdit com_rx_counter;
CEdit com_tx_counter;

unsigned long com_tx;
unsigned long com_rx;


int CUartandTcpTestDlg::Com_Init(void)
{
	if( 0 != Enum_Serial() )
	{
		port.SetCurSel(0);
	}
	else
	{
		AfxMessageBox("No Serial Port！");
	}

	port.SetCurSel(0);
	baudrate.SetCurSel(0);
	databit.SetCurSel(0);
	parity.SetCurSel(0);
	stopbit.SetCurSel(0);

	ComState = COM_CLOSED;
	hrcvThread = NULL;

	com_rx = com_tx = 0;
	com_rx_counter.SetWindowTextA("0");
	com_tx_counter.SetWindowTextA("0");

	com_rx_format = ASCII;

	return 0;
}
/*
	枚举串口
*/
int CUartandTcpTestDlg::Enum_Serial(void)
{
	CString  strSerialList[256]; // 临时定义 256 个字符串组，因为系统最多也就 256 个 

	HKEY hKey;  

	LPCTSTR data_Set="HARDWARE\\DEVICEMAP\\SERIALCOMM\\";  

	long ret0 = (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey)); 

	if(ret0 != ERROR_SUCCESS)  
	{  
		return -1;  
	}  
	int i=0;
	CHAR Name[25];  
	UCHAR szPortName[25];  
	LONG Status;  
	DWORD dwIndex = 0;  
	DWORD dwName;  
	DWORD dwSizeofPortName;  
	DWORD Type;  
	dwName = sizeof(Name);  
	dwSizeofPortName = sizeof(szPortName);

	do  
	{  
		Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, NULL, &Type,  
			szPortName, &dwSizeofPortName);  
		if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA))  
		{  
			port.AddString((LPCTSTR)szPortName);   // 串口字符串保存  
			i++;// 串口计数  
		}  
	} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA)); 

	RegCloseKey(hKey);

	return i;
}

/*串口接收线程*/
DWORD WINAPI serial_rcv(void *arg)
{
	char	str[2048]={0};
	int		index;
	int		len = 0;

	while( ComState != COM_CLOSED )
	{
		memset(str,0,sizeof(str));
		len = myserial.Read(str,512);

		if( HEX == com_rx_format )
		{
			char *p = str;
			char buf[512] = {0};
			memcpy(buf,str,512);
			for( int i =0; i< len ; i++)
			{
				sprintf(p,"%02X ",buf[i]);
				p += 3;
			}
		}

		index = com_rcv_txt.GetWindowTextLengthA();
		com_rcv_txt.SetSel(index, index);
		com_rcv_txt.ReplaceSel(str);

		//计数
		com_rx += len;
		com_rx_counter.SetWindowTextA(ltoa(com_rx,str,10));
	}

	ComState = COM_CLOSED;
	return 0;
}

//打开串口按钮
void CUartandTcpTestDlg::OnBnClickedButton1()
{
	if( COM_CLOSED == ComState )
	{
		MyUart comset;
		port.GetWindowTextA(comset.port_name,255);

		switch( baudrate.GetCurSel() )
		{
		case 0:
			comset.dcb.BaudRate = 115200;
			break;
		case 1:
			comset.dcb.BaudRate = 57600;
			break;
		case 2:
			comset.dcb.BaudRate = 38400;
			break;
		case 3:
			comset.dcb.BaudRate = 19200;
			break;
		case 4:
			comset.dcb.BaudRate = 9600;
			break;
		case 5:
			comset.dcb.BaudRate = 4800;
			break;
		case 6:
			comset.dcb.BaudRate = 1200;
			break;
		default:
			comset.dcb.BaudRate = 115200;
			break;
		}

		switch( databit.GetCurSel() )
		{
		case 0:
			comset.dcb.ByteSize = 8;
			break;
		case 1:
			comset.dcb.ByteSize = 7;
			break;
		default:
			comset.dcb.ByteSize = 8;
			break;
		}

		switch( parity.GetCurSel() )
		{
		case 0:
			comset.dcb.Parity = 0;
			break;
		case 1:
			comset.dcb.Parity = 1;
			break;
		case 2:
			comset.dcb.Parity = 2;
			break;
		default:
			comset.dcb.Parity = 0;
			break;
		}

		switch( stopbit.GetCurSel() )
		{
		case 0:
			comset.dcb.StopBits = 1;
			break;
		case 1:
			comset.dcb.StopBits = 2;
			break;
		default:
			comset.dcb.StopBits = 1;
			break;
		}

		//打开串口
		if( !myserial.Open(&comset) )
		{
			opencom.SetWindowTextA("Close");
			//如果打开成功
			ComState = COM_OPEN;

			hrcvThread = ::CreateThread(NULL,0,serial_rcv,NULL,0,NULL);

			if( NULL == hrcvThread )
			{
				AfxMessageBox("Create COM RX Thread failed！");
				ExitProcess(-1);
			}
		}
		else
		{
			AfxMessageBox("Open COM failed!");
		}
	}
	else
	{

		if( com_snd_cycle.GetCheck() )
		{
			//停止循环发送
			com_timer_msg= true;
			this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK2,BN_CLICKED), NULL);
		}

		//关闭串口
		if( !myserial.Close() )//如果关闭成功
		{	
			ComState = COM_CLOSED;
			Sleep(10);
			TerminateThread(hrcvThread,0);
			CloseHandle(hrcvThread);
			
			opencom.SetWindowTextA("Open");
		}
		else
		{
			AfxMessageBox("Close COM failed！");
		}
	}
}

//clear 按钮
void CUartandTcpTestDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	com_rcv_txt.SetWindowTextA("");
	com_rx = com_tx = 0;
	com_rx_counter.SetWindowTextA("0");
	com_tx_counter.SetWindowTextA("0");
}

//发送按钮
void CUartandTcpTestDlg::OnBnClickedButton2()
{

	char buffer[1024]={0};
	int len = 0;

	if( COM_OPEN == ComState )
	{

		len = com_send_txt.GetWindowTextA(buffer,sizeof(buffer));

		if( com_snd_hex.GetCheck() )
		{
			CString s= buffer;
			//如果是16进制发送的话，需要删掉空格
			s.Remove(' ');
			memset(buffer,0,sizeof(buffer));
			len = ::CharToHex((unsigned char *)s.GetBuffer(),(unsigned char *)buffer,sizeof(buffer));
		}

		if( com_snd_new_line.GetCheck() )
		{
			strcat(buffer,"\r\n");
			len+=2;
		}

		myserial.Write(buffer,len);
		//计数
		com_tx += len;
		com_tx_counter.SetWindowTextA(ltoa(com_tx,buffer,10));

	}
	else
	{
		AfxMessageBox("COM Not Open！");
	}

}

//循环发送事件
void WINAPI com_timer(HWND,UINT,UINT_PTR,DWORD)
{
	if( COM_OPEN == ComState )
		SendMessage(AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON2,BN_CLICKED), NULL);
}

const int com_timer_id = 8;

//定时发送的
void CUartandTcpTestDlg::OnBnClickedCheck2()
{
	char buf[10]={0};
	int		time = 0;

	if( (COM_OPEN == ComState) && ( !com_timer_msg) && com_snd_cycle.GetCheck() )
	{		
		//如果为空
		if( 0 == com_snd_cycle_tm.GetWindowTextA(buf,10) )
		{
			com_snd_cycle.SetCheck(false);
			return;
		}
		//转换时间
		time = atoi(buf);
		//时间限制
		if( time > 100000 || time < 0 )
		{
			AfxMessageBox("Error");
			com_snd_cycle.SetCheck(false);
		}
		else
		{
			if( com_timer_id != com_snd_cycle.SetTimer(com_timer_id,time,com_timer) )
			{
				AfxMessageBox("SetTimer Failed！");
			}
		}

	}
	else
	{
		//狗屎程序，发消息用的
		com_timer_msg = false;
		com_snd_cycle.KillTimer(com_timer_id);
		com_snd_cycle.SetCheck(false);
	}
}

//hex显示按钮
void CUartandTcpTestDlg::OnBnClickedCheck8()
{
	// TODO: 在此添加控件通知处理程序代码
	if( m_com_rx_format.GetCheck() )
	{
		com_rx_format = HEX;
	}
	else
	{
		com_rx_format = ASCII;
	}
}


