#include "StdAfx.h"
#include "Uart and Tcp Test.h"
#include "Uart and Tcp TestDlg.h"
#include "afxdialogex.h"
#include "NetWorkView.h"
#include "NetWork.h"
#include "Func.h"

#define NET_CLOSED		0
#define NET_CONNECT		1
#define NET_LISTEN		2
#define NET_CONTROL		3

CEdit		net_rcv_txt;
int			NetState;
int			net_rx_format;
HANDLE		hNetRcvThread;
NetWork		mynet;

bool		timer_msg = false;

CEdit		net_rx_counter;
CEdit		net_tx_counter;

u_long		net_rx;
u_long		net_tx;

int CUartandTcpTestDlg::Net_Init(void)
{
	//网络状态
	NetState = NET_CLOSED;
	hNetRcvThread = NULL;

	localportchk.SetCheck(1);
	localport.SetWindowTextA("8000");
	//网络协议
	proto_c_s.SetCurSel(0);
	remote_ip.EnableWindow(false);

	net_rx = net_tx = 0;

	net_rx_counter.SetWindowTextA("0");
	net_tx_counter.SetWindowTextA("0");

	net_rx_format = ASCII;

	return 0;
}

//网络接收线程
DWORD WINAPI net_rcv(void *arg)
{
	char	str[2048]={0};
	int		index;
	int		len;

	while( NetState != NET_CLOSED )
	{
		memset(str,0,sizeof(str));

		len = mynet.Read(str,512);

		if( 0 == len )
		{
			timer_msg = true;
			//发消息给循环发送按钮
			SendMessage(AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_CHECK5,BN_CLICKED), NULL);
			//发送消息给断开按钮
			SendMessage(AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON6,BN_CLICKED), NULL);
			break;
		}

		if( HEX == net_rx_format )
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

		index = net_rcv_txt.GetWindowTextLengthA();
		net_rcv_txt.SetSel(index, index);
		net_rcv_txt.ReplaceSel(str);
		//计数器更新
		net_rx += len;
		net_rx_counter.SetWindowTextA(ltoa(net_rx,str,10));
	}

	//设置网络已断开
	NetState = NET_CLOSED;

	return 0;
}

//等待连接的一个过程
DWORD WINAPI get_accept(void *arg)
{
	if( -1 != mynet.GetAccept() )
	{
		NetState = NET_CONTROL;
		//发送消息
		SendMessage( AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON6,BN_CLICKED), NULL );
	}

	return 0;
}

//监听 or 打开按钮
void CUartandTcpTestDlg::OnBnClickedButton6()
{
	MyHost  m_host = {0};
	int		mode = -1;
	
	char    remoteip[20] = {0};
	char	remoteport[16] = {0};

	if( NetState == NET_CLOSED )
	{
		//打开
		switch( proto_c_s.GetCurSel() )
		{
		case 0:
			//TCP_SERVER
			mode  = TCP_SERVER;
			break;
		case 1:
			//TCP_CLIENT
			mode = TCP_CLIENT;
			break;
		case 2:
			//UDP_SERVER
			mode = UDP_SERVER;
			break;
		case 3:
			//UDP_CLIENT
			mode = UDP_CLIENT;
			break;
		default:
			AfxMessageBox("Please choose the protocol！");
			break;
		}

		//客户端模式
		if( (TCP_CLIENT == mode) || (UDP_CLIENT == mode) )
		{
			//对方主机信息
			remote_ip.GetWindowTextA(m_host.ip,sizeof(m_host.ip));
	
			if( 0 == remote_port.GetWindowTextA(remoteport,sizeof(remoteport)) )
			{
				AfxMessageBox("remote port can't empty！");
				return;
			}
			//不需要转换字节顺序
			m_host.dstport = atoi(remoteport);
		}
		//服务器模式
		if( (TCP_SERVER == mode) || (UDP_SERVER == mode) )
		{
			if( 0 == localport.GetWindowTextA(remoteport,sizeof(remoteport)) )
			{
				AfxMessageBox("port can't empty！");
				return;
			}
			m_host.bind_local_port = true;
			//监听的本机
			m_host.localport = atoi(remoteport);//不需要转换字节顺序
		}
		else if( localportchk.GetCheck() )
		{
			char port[16] = {0};
			localport.GetWindowTextA(port,sizeof(port));
			m_host.bind_local_port = true;
			m_host.localport = atoi(port);//不需要转换字节顺序
		}
		/*要不加一个getsocket函数？？*/
		if( -1 == mynet.Create(&m_host,mode) )
		{
			//失败
			AfxMessageBox("Create NetWork Failed!");
			mynet.Close();
		}
		else
		{
			
			//创建一个TCP or UDP accept函数
			if( TCP_SERVER == mode )
			{
				CreateThread(NULL,0,get_accept,NULL,0,NULL);
				//按钮显示断开
				net_open.SetWindowTextA("Waitting");
				//网络状态为连接
				NetState = NET_LISTEN;
			}
			else
			{
				//网络状态为连接
				NetState = NET_CONNECT;
			}

			if( NET_CONNECT == NetState ) 
			{				
				//按钮显示断开
				net_open.SetWindowTextA("Disconnect");
				//创建socket接收线程
				hNetRcvThread = ::CreateThread(NULL,0,net_rcv,NULL,0,NULL);
				if( NULL == hNetRcvThread )
				{
					AfxMessageBox("Create NetWork RX Thread Failed！");
					ExitProcess(-1);
				}
			}

		}

	}
	else if( NET_CONTROL == NetState )
	{
		NetState = NET_CONNECT;
		net_open.SetWindowTextA("Disconnect");
		//创建socket接收线程
		if( NULL ==  hNetRcvThread )
			hNetRcvThread = ::CreateThread(NULL,0,net_rcv,NULL,0,NULL);
		if( NULL == hNetRcvThread )
		{
			AfxMessageBox("Create NetWork RX Thread Failed！");
			ExitProcess(-1);
		}
	}
	else //关闭
	{	
		if( net_cycle_snd.GetCheck() )
		{
			//停止循环发送
			timer_msg= true;
			this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK5,BN_CLICKED), NULL);
		}

		//网络状态为断开
		NetState = NET_CLOSED;
		Sleep(10);
		//关闭接收线程
		TerminateThread(hNetRcvThread,0);
		CloseHandle(hNetRcvThread);
		hNetRcvThread  = NULL;
		mynet.Close();
		//按钮的文字
		int select = proto_c_s.GetCurSel();
		if( (0 == select) || (2 == select) )
			net_open.SetWindowTextA("Listen");
		else
			net_open.SetWindowTextA("Connect");
	}
}

//clear 按钮
void CUartandTcpTestDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	net_rcv_txt.SetWindowTextA("");
	//计数清零
	net_rx = net_tx = 0;
	net_tx_counter.SetWindowTextA("0");
	net_rx_counter.SetWindowTextA("0");
}

//发送按钮
void CUartandTcpTestDlg::OnBnClickedButton3()
{
	//网络发送
	char buffer[1024] = {0};
	int  len = 0;

	if( NET_CONNECT == NetState )
	{
		len = net_snd_txt.GetWindowTextA(buffer,sizeof(buffer));

		if( net_snd_hex.GetCheck() )
		{
			CString s= buffer;
			//如果是16进制发送的话，需要删掉空格
			s.Remove(' ');
			memset(buffer,0,sizeof(buffer));
			len = ::CharToHex((unsigned char *)s.GetBuffer(),(unsigned char *)buffer,sizeof(buffer));
		}

		if( net_snd_new_line.GetCheck() )
		{
			strcat(buffer,"\r\n");
			len+=2;
		}

		len = mynet.Write(buffer,len);
		//网络发送
		if( -1 ==  len )
		{
			AfxMessageBox("Send Failed！");
			return;
		}

		//计数器
		net_tx += len;
		net_tx_counter.SetWindowTextA(ltoa(net_tx,buffer,10));

	}
	else
	{
		//米有连接
		AfxMessageBox("NetWork Not Connect！");
	}
}

void CUartandTcpTestDlg::OnCbnSelchangeCombo6()
{
	// TODO: 在此添加控件通知处理程序代码
	if( 0 == proto_c_s.GetCurSel() || 2 == proto_c_s.GetCurSel() )
	{
		net_open.SetWindowTextA("Listen");
		remote_ip.EnableWindow(false);
		remote_port.EnableWindow(false);
		localportchk.SetCheck(1);
		localport.EnableWindow(true);
	}
	else
	{
		net_open.SetWindowTextA("Connect");
		remote_ip.EnableWindow(true);
		remote_port.EnableWindow(true);
		localportchk.SetCheck(0);
		localport.EnableWindow(false);
	}

}

//循环发送事件
void WINAPI net_timer(HWND,UINT,UINT_PTR,DWORD)
{
	SendMessage(AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON3,BN_CLICKED), NULL);
}

//timer id
const int net_timer_id = 6; 
//循环发送事件
void CUartandTcpTestDlg::OnBnClickedCheck5()
{
	char buf[10]={0};
	int		time = 0;

	if( (NET_CONNECT == NetState ) && ( !timer_msg) && net_cycle_snd.GetCheck() )
	{
		//如果为空
		if( 0 == net_snd_cycle_tm.GetWindowTextA(buf,10) )
		{
			net_cycle_snd.SetCheck(false);
			return;
		}
		//转换时间
		time = atoi(buf);
		//时间限制
		if( time > 100000 || time < 0 )
		{
			AfxMessageBox("Error");
			net_cycle_snd.SetCheck(false);
		}
		else
		{
			if( net_timer_id != net_cycle_snd.SetTimer(net_timer_id,time,net_timer) )
			{
				AfxMessageBox("SetTimer Failed！");
			}
		}
	}
	else
	{
		//为了发消息好用而已
		timer_msg = false;

		net_cycle_snd.KillTimer(net_timer_id);
		net_cycle_snd.SetCheck(false);
	}

}

//hex 显示按钮
void CUartandTcpTestDlg::OnBnClickedCheck9()
{
	// TODO: 在此添加控件通知处理程序代码
	if( m_net_rx_format.GetCheck() )
	{
		net_rx_format = HEX;
	}
	else
	{
		net_rx_format = ASCII;
	}
}
