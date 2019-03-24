// DSOInjection.cpp: 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "DSOInjection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static double value = 0.0;

CDSOInjectionApp theApp;

extern "C" void OnValueReceived(double value) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	char buffer[256] = { 0 };

	snprintf(buffer, sizeof(buffer), "F=%f\n", value);
	theApp.SendSocketMessage(buffer, sizeof(buffer));
}

extern "C" __declspec(naked) BOOL PASCAL DummyFunction()
{
	//__asm mov fs : [0], eax;//64 A3 00 00 00 00 (0001A5BF:file pos)
	//call DummyFunction; //FF 93 62 C1 32 00
	__asm
	{
		mov fs : [0], eax;
		fld qword ptr[ebp + 0x18];
		fstp value;
	}
	OnValueReceived(value);

	__asm ret;
}

//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CDSOInjectionApp

BEGIN_MESSAGE_MAP(CDSOInjectionApp, CWinApp)
END_MESSAGE_MAP()
//插入DLL方法：
//用Stu_PE，插入DLL以及要使用的函数
//但是要注意的是，函数地址实际上在插入位置之后的4个字节。而插入位置为00 00 00 00
//根据情况确定是否在这4个0的位置上复制函数地址的RVA。

//重定位方法：
//分别扩展.reloc段和重定位表的大小，使其可以容纳新的重定位数据

//在.reloc段中找到最后的0（4字节对齐），
//增加RVA地址：XX XX XX XX，后12位留0（00 0X XX XX）
//下4个字节写总长，包括前面的4个字节和自身的4个字节，以及最后4个字节的0，
//应该考虑至少为 10 00 00 00 （16）
//下2个字节写 3X XX，高位在前，为12位。
//可能还有更多
//最后4个字节写 00 00 00 00


CDSOInjectionApp::CDSOInjectionApp()
	:SenderSocket(INVALID_SOCKET)
	,SocketAddress()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CDSOInjectionApp 对象


// CDSOInjectionApp 初始化

BOOL CDSOInjectionApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	this->SenderSocket = socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP);
	if (this->SenderSocket != INVALID_SOCKET) {
		this->SocketAddress.sin_family = AF_INET;
		this->SocketAddress.sin_port = htons(8192);
		this->SocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	return TRUE;
}
int CDSOInjectionApp::ExitInstance()
{
	if (this->SenderSocket != INVALID_SOCKET) {
		closesocket(this->SenderSocket);
		this->SenderSocket = INVALID_SOCKET;
	}
	return CWinApp::ExitInstance();
}

void CDSOInjectionApp::SendSocketMessage(char * text, int length)
{
	if (this->SenderSocket != INVALID_SOCKET && text != 0 && length > 0) {
		sendto(this->SenderSocket, text, length, 0, (const sockaddr*)&this->SocketAddress, sizeof(this->SocketAddress));
	}
}
