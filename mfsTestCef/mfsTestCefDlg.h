
// mfsTestCefDlg.h: 头文件
//

#pragma once

#include "common.h"


// CmfsTestCefDlg 对话框
class CmfsTestCefDlg : public CDialogEx
{
// 构造
public:
	CmfsTestCefDlg(CWnd* pParent = nullptr);	// 标准构造函数
	void MyDoModal();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFSTESTCEF_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	// 在类的成员变量区域添加
	CToolBar m_wndToolBar;  // 工具条对象
	HICON m_hIcon;
	bool bInited = false;
	// 加这两个
	


	bool init();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnToolCmd1();
	afx_msg void OnToolCmd2();
	afx_msg void OnToolCmd3();
	afx_msg void OnToolCmd4();
	CStatic mContainer;
};
