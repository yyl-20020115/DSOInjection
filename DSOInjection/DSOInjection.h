// DSOInjection.h: DSOInjection DLL 的主标头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CDSOInjectionApp
// 有关此类实现的信息，请参阅 DSOInjection.cpp
//

class CDSOInjectionApp : public CWinApp
{
public:
	CDSOInjectionApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void SendSocketMessage(char* text, int length);
	DECLARE_MESSAGE_MAP()
protected:
	SOCKET SenderSocket;
	sockaddr_in SocketAddress;
};
