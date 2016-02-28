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
	//����״̬
	NetState = NET_CLOSED;
	hNetRcvThread = NULL;

	localportchk.SetCheck(1);
	localport.SetWindowTextA("8000");
	//����Э��
	proto_c_s.SetCurSel(0);
	remote_ip.EnableWindow(false);

	net_rx = net_tx = 0;

	net_rx_counter.SetWindowTextA("0");
	net_tx_counter.SetWindowTextA("0");

	net_rx_format = ASCII;

	return 0;
}

//��������߳�
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
			//����Ϣ��ѭ�����Ͱ�ť
			SendMessage(AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_CHECK5,BN_CLICKED), NULL);
			//������Ϣ���Ͽ���ť
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
		//����������
		net_rx += len;
		net_rx_counter.SetWindowTextA(ltoa(net_rx,str,10));
	}

	//���������ѶϿ�
	NetState = NET_CLOSED;

	return 0;
}

//�ȴ����ӵ�һ������
DWORD WINAPI get_accept(void *arg)
{
	if( -1 != mynet.GetAccept() )
	{
		NetState = NET_CONTROL;
		//������Ϣ
		SendMessage( AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON6,BN_CLICKED), NULL );
	}

	return 0;
}

//���� or �򿪰�ť
void CUartandTcpTestDlg::OnBnClickedButton6()
{
	MyHost  m_host = {0};
	int		mode = -1;
	
	char    remoteip[20] = {0};
	char	remoteport[16] = {0};

	if( NetState == NET_CLOSED )
	{
		//��
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
			AfxMessageBox("Please choose the protocol��");
			break;
		}

		//�ͻ���ģʽ
		if( (TCP_CLIENT == mode) || (UDP_CLIENT == mode) )
		{
			//�Է�������Ϣ
			remote_ip.GetWindowTextA(m_host.ip,sizeof(m_host.ip));
	
			if( 0 == remote_port.GetWindowTextA(remoteport,sizeof(remoteport)) )
			{
				AfxMessageBox("remote port can't empty��");
				return;
			}
			//����Ҫת���ֽ�˳��
			m_host.dstport = atoi(remoteport);
		}
		//������ģʽ
		if( (TCP_SERVER == mode) || (UDP_SERVER == mode) )
		{
			if( 0 == localport.GetWindowTextA(remoteport,sizeof(remoteport)) )
			{
				AfxMessageBox("port can't empty��");
				return;
			}
			m_host.bind_local_port = true;
			//�����ı���
			m_host.localport = atoi(remoteport);//����Ҫת���ֽ�˳��
		}
		else if( localportchk.GetCheck() )
		{
			char port[16] = {0};
			localport.GetWindowTextA(port,sizeof(port));
			m_host.bind_local_port = true;
			m_host.localport = atoi(port);//����Ҫת���ֽ�˳��
		}
		/*Ҫ����һ��getsocket��������*/
		if( -1 == mynet.Create(&m_host,mode) )
		{
			//ʧ��
			AfxMessageBox("Create NetWork Failed!");
			mynet.Close();
		}
		else
		{
			
			//����һ��TCP or UDP accept����
			if( TCP_SERVER == mode )
			{
				CreateThread(NULL,0,get_accept,NULL,0,NULL);
				//��ť��ʾ�Ͽ�
				net_open.SetWindowTextA("Waitting");
				//����״̬Ϊ����
				NetState = NET_LISTEN;
			}
			else
			{
				//����״̬Ϊ����
				NetState = NET_CONNECT;
			}

			if( NET_CONNECT == NetState ) 
			{				
				//��ť��ʾ�Ͽ�
				net_open.SetWindowTextA("Disconnect");
				//����socket�����߳�
				hNetRcvThread = ::CreateThread(NULL,0,net_rcv,NULL,0,NULL);
				if( NULL == hNetRcvThread )
				{
					AfxMessageBox("Create NetWork RX Thread Failed��");
					ExitProcess(-1);
				}
			}

		}

	}
	else if( NET_CONTROL == NetState )
	{
		NetState = NET_CONNECT;
		net_open.SetWindowTextA("Disconnect");
		//����socket�����߳�
		if( NULL ==  hNetRcvThread )
			hNetRcvThread = ::CreateThread(NULL,0,net_rcv,NULL,0,NULL);
		if( NULL == hNetRcvThread )
		{
			AfxMessageBox("Create NetWork RX Thread Failed��");
			ExitProcess(-1);
		}
	}
	else //�ر�
	{	
		if( net_cycle_snd.GetCheck() )
		{
			//ֹͣѭ������
			timer_msg= true;
			this->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK5,BN_CLICKED), NULL);
		}

		//����״̬Ϊ�Ͽ�
		NetState = NET_CLOSED;
		Sleep(10);
		//�رս����߳�
		TerminateThread(hNetRcvThread,0);
		CloseHandle(hNetRcvThread);
		hNetRcvThread  = NULL;
		mynet.Close();
		//��ť������
		int select = proto_c_s.GetCurSel();
		if( (0 == select) || (2 == select) )
			net_open.SetWindowTextA("Listen");
		else
			net_open.SetWindowTextA("Connect");
	}
}

//clear ��ť
void CUartandTcpTestDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	net_rcv_txt.SetWindowTextA("");
	//��������
	net_rx = net_tx = 0;
	net_tx_counter.SetWindowTextA("0");
	net_rx_counter.SetWindowTextA("0");
}

//���Ͱ�ť
void CUartandTcpTestDlg::OnBnClickedButton3()
{
	//���緢��
	char buffer[1024] = {0};
	int  len = 0;

	if( NET_CONNECT == NetState )
	{
		len = net_snd_txt.GetWindowTextA(buffer,sizeof(buffer));

		if( net_snd_hex.GetCheck() )
		{
			CString s= buffer;
			//�����16���Ʒ��͵Ļ�����Ҫɾ���ո�
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
		//���緢��
		if( -1 ==  len )
		{
			AfxMessageBox("Send Failed��");
			return;
		}

		//������
		net_tx += len;
		net_tx_counter.SetWindowTextA(ltoa(net_tx,buffer,10));

	}
	else
	{
		//��������
		AfxMessageBox("NetWork Not Connect��");
	}
}

void CUartandTcpTestDlg::OnCbnSelchangeCombo6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

//ѭ�������¼�
void WINAPI net_timer(HWND,UINT,UINT_PTR,DWORD)
{
	SendMessage(AfxGetApp()->GetMainWnd()->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON3,BN_CLICKED), NULL);
}

//timer id
const int net_timer_id = 6; 
//ѭ�������¼�
void CUartandTcpTestDlg::OnBnClickedCheck5()
{
	char buf[10]={0};
	int		time = 0;

	if( (NET_CONNECT == NetState ) && ( !timer_msg) && net_cycle_snd.GetCheck() )
	{
		//���Ϊ��
		if( 0 == net_snd_cycle_tm.GetWindowTextA(buf,10) )
		{
			net_cycle_snd.SetCheck(false);
			return;
		}
		//ת��ʱ��
		time = atoi(buf);
		//ʱ������
		if( time > 100000 || time < 0 )
		{
			AfxMessageBox("Error");
			net_cycle_snd.SetCheck(false);
		}
		else
		{
			if( net_timer_id != net_cycle_snd.SetTimer(net_timer_id,time,net_timer) )
			{
				AfxMessageBox("SetTimer Failed��");
			}
		}
	}
	else
	{
		//Ϊ�˷���Ϣ���ö���
		timer_msg = false;

		net_cycle_snd.KillTimer(net_timer_id);
		net_cycle_snd.SetCheck(false);
	}

}

//hex ��ʾ��ť
void CUartandTcpTestDlg::OnBnClickedCheck9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( m_net_rx_format.GetCheck() )
	{
		net_rx_format = HEX;
	}
	else
	{
		net_rx_format = ASCII;
	}
}
