
// Uart and Tcp TestDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "explorer1.h"
#include "afxcmn.h"

#define ASCII	1
#define HEX		2

// CUartandTcpTestDlg 对话框
class CUartandTcpTestDlg : public CDialogEx
{
// 构造
public:
	CUartandTcpTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UARTANDTCPTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG*   pMsg);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	int UserInit(void);

protected:
	CComboBox	port;
	CComboBox	baudrate;
	CComboBox	databit;
	CComboBox	parity;
	CComboBox	stopbit;
	CButton		opencom;
	CEdit		com_send_txt;
	CButton		com_snd_new_line;
	CButton		com_snd_hex;

	CButton		localportchk;
	CButton		net_open;
	CEdit		remote_port;
	CIPAddressCtrl remote_ip;
	CComboBox	proto_c_s;
	CEdit		localport;
	CButton		net_snd_hex;
	CButton		net_snd_new_line;
	CEdit		net_snd_txt;

	CExplorer1	html_ctrl;

	CButton net_cycle_snd;
	CEdit net_snd_cycle_tm;
	CButton com_snd_cycle;
	CEdit com_snd_cycle_tm;

protected:
	int Logo_Init(void);
	int Com_Init(void);
	int Net_Init(void);

private:
	int Enum_Serial(void);

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedMfcbutton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnCbnSelchangeCombo6();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck5();

	afx_msg void OnBnClickedCheck8();
	afx_msg void OnBnClickedCheck9();
protected:
	CButton m_com_rx_format;
	CButton m_net_rx_format;
};
