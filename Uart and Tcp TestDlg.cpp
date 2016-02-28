
// Uart and Tcp TestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Uart and Tcp Test.h"
#include "Uart and Tcp TestDlg.h"
#include "afxdialogex.h"
//------------------------
#include "MySerialView.h"
#include "NetWorkView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//������
extern CEdit		com_rcv_txt;
extern CEdit		net_rcv_txt;

//������
extern CEdit		com_rx_counter;
extern CEdit		com_tx_counter;

extern CEdit		net_rx_counter;
extern CEdit		net_tx_counter;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUartandTcpTestDlg �Ի���

CUartandTcpTestDlg::CUartandTcpTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUartandTcpTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUartandTcpTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, port);
	DDX_Control(pDX, IDC_COMBO2, baudrate);
	DDX_Control(pDX, IDC_COMBO3, databit);
	DDX_Control(pDX, IDC_COMBO4, parity);
	DDX_Control(pDX, IDC_COMBO5, stopbit);
	DDX_Control(pDX, IDC_COMBO6, proto_c_s);
	DDX_Control(pDX, IDC_EDIT9, localport);
	DDX_Control(pDX, IDC_CHECK7, localportchk);
	DDX_Control(pDX, IDC_EXPLORER1, html_ctrl);
	DDX_Control(pDX, IDC_BUTTON1, opencom);
	DDX_Control(pDX, IDC_BUTTON6, net_open);
	DDX_Control(pDX, IDC_EDIT8, remote_port);
	DDX_Control(pDX, IDC_IPADDRESS1, remote_ip);
	DDX_Control(pDX, IDC_EDIT1, com_rcv_txt);
	DDX_Control(pDX, IDC_EDIT3, com_send_txt);
	DDX_Control(pDX, IDC_EDIT2, net_rcv_txt);
	DDX_Control(pDX, IDC_CHECK3, com_snd_new_line);
	DDX_Control(pDX, IDC_CHECK1, com_snd_hex);
	DDX_Control(pDX, IDC_CHECK4, net_snd_hex);
	DDX_Control(pDX, IDC_CHECK6, net_snd_new_line);
	DDX_Control(pDX, IDC_EDIT4, net_snd_txt);
	DDX_Control(pDX, IDC_CHECK5, net_cycle_snd);
	DDX_Control(pDX, IDC_EDIT6, net_snd_cycle_tm);
	DDX_Control(pDX, IDC_CHECK2, com_snd_cycle);
	DDX_Control(pDX, IDC_EDIT5, com_snd_cycle_tm);
	DDX_Control(pDX, IDC_EDIT7, com_rx_counter);
	DDX_Control(pDX, IDC_EDIT10, com_tx_counter);
	DDX_Control(pDX, IDC_EDIT11, net_rx_counter);
	DDX_Control(pDX, IDC_EDIT12, net_tx_counter);
	DDX_Control(pDX, IDC_CHECK8, m_com_rx_format);
	DDX_Control(pDX, IDC_CHECK9, m_net_rx_format);
}

BEGIN_MESSAGE_MAP(CUartandTcpTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUartandTcpTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CUartandTcpTestDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CUartandTcpTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON5, &CUartandTcpTestDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CUartandTcpTestDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_MFCBUTTON1, &CUartandTcpTestDlg::OnBnClickedMfcbutton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CUartandTcpTestDlg::OnBnClickedButton3)
	ON_CBN_SELCHANGE(IDC_COMBO6, &CUartandTcpTestDlg::OnCbnSelchangeCombo6)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK2, &CUartandTcpTestDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK5, &CUartandTcpTestDlg::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK8, &CUartandTcpTestDlg::OnBnClickedCheck8)
	ON_BN_CLICKED(IDC_CHECK9, &CUartandTcpTestDlg::OnBnClickedCheck9)
END_MESSAGE_MAP()


// CUartandTcpTestDlg ��Ϣ�������

BOOL CUartandTcpTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	UserInit();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CUartandTcpTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUartandTcpTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUartandTcpTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//�û���ʼ��
int CUartandTcpTestDlg::UserInit(void)
{
	if( Com_Init() )
	{
		AfxMessageBox("COM Initialize Failed��");
	}

	if( Net_Init() )
	{
		AfxMessageBox("NetWork Initialize Failed��");
	}

	Logo_Init();

	return 0;
}

//logo ��ʼ��
int CUartandTcpTestDlg::Logo_Init(void)
{
	TCHAR strCurDrt[500];
	int nLen = ::GetCurrentDirectory(500,strCurDrt);
	if( strCurDrt[nLen]!='\\' )
	{
		strCurDrt[nLen++] = '\\';
		strCurDrt[nLen] = '\0';
	}

	CString strFileName = strCurDrt;
	strFileName += "config\\logo.html"; //����Ϊ��ľ���·��

	if( NULL != fopen(strFileName,"rb") ) 
		html_ctrl.Navigate(strFileName,NULL,NULL,NULL,NULL);
	else
		html_ctrl.Navigate("http://www.hlktech.com/",NULL,NULL,NULL,NULL);

	return 0;
}

void CUartandTcpTestDlg::OnBnClickedMfcbutton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxMessageBox("ShenZhen Hi-Link Electronic Co.,Ltd  www.hlktech.com");
}

//�����˳���Ϣ
void CUartandTcpTestDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if( AfxMessageBox( "are you sure��",MB_OKCANCEL) == IDOK) 
	{
		//����һ��������Դ
		/*****************/
		CDialogEx::OnClose();
	}
}

//���ط��Ϳ�Ļس�����
BOOL CUartandTcpTestDlg::PreTranslateMessage(MSG*   pMsg)
{
	CWnd *p = this->GetFocus();
	int id;

	if( NULL != p )
		id = p->GetDlgCtrlID();

	if( id == IDC_EDIT3 || id == IDC_EDIT4 )
	{

		if( (WM_KEYDOWN == pMsg->message) && (VK_RETURN == pMsg->wParam) )
		{
			//���ص���Ϣ
			if( id == IDC_EDIT3 )
			{
				::PostMessage(this->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON2,BN_CLICKED), NULL);
			}
			else if( id == IDC_EDIT4 )
			{
				::PostMessage(this->m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_BUTTON3,BN_CLICKED), NULL);
			}

		}
		else
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
	}
	else
	{
		return CDialogEx::PreTranslateMessage(pMsg);
	}

	return true;
}