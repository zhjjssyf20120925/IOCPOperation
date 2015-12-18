#include "stdafx.h"
#include "IocpServerOperation.h"

static bool bInit = false;


/***********************************************************************************************************
* 程序作者：赵进军
* 函数功能：构造函数
* 参数说明：null
* 注意事项：null
* 修改日期：2015/12/18 16:37:00
***********************************************************************************************************/
IocpServerOperation::IocpServerOperation()
{
}


/***********************************************************************************************************
* 程序作者：赵进军
* 函数功能：析构函数
* 参数说明：null
* 注意事项：null
* 修改日期：2015/12/18 16:38:00
***********************************************************************************************************/
IocpServerOperation::~IocpServerOperation()
{
}


/***********************************************************************************************************
 * 程序作者：赵进军
 * 函数功能：工作线程
 * 参数说明：
 *    Param：指针类型的参数
 * 注意事项：null
 * 修改日期：2015/12/18 16:45:00
 ***********************************************************************************************************/
DWORD __stdcall IocpServerOperation::WorkThread(LPVOID Param)
{
	IocpServerOperation* pthis = (IocpServerOperation*)Param;
	void* re;
	OVERLAPPED* pOverlap;
	DWORD berByte;
	while (pthis->m_workThread)
	{
		int ret;
		ret = GetQueuedCompletionStatus(pthis->m_hIocp, &berByte, (LPDWORD)&re, (LPOVERLAPPED*)&pOverlap, INFINITE);
		if (ret == ERROR_SUCCESS)
		{

		}

		if (berByte == 0)
		{
			pthis->OnClose(re);																						/// 客户端断开连接
			OVERLAPPEDPLUS* olp = (OVERLAPPEDPLUS*)pOverlap;
			closesocket(olp->s);
			delete olp;																								/// 释放与socket绑定的结构体变量
			continue;
		}

		if (NULL == re)
			return 0;
		OVERLAPPEDPLUS *olp = (OVERLAPPEDPLUS*)pOverlap;
		switch (olp->OpCode)
		{
		case OP_READ:
			pthis->OnRead(re, olp->wbuf.buf, berByte);																/// 调用 Onread() 通知应用程序，服务器收到来自客户端的网络数据
			WSARecv(olp->s, &olp->wbuf, 1, &olp->dwBytes, &olp->dwFlags, &olp->ol, NULL);							/// 继续调用一个接受的I/O异步请求
			break;
		default:
			break;
		}
	}
	return 0;
}


/***********************************************************************************************************
 * 程序作者：赵进军
 * 函数功能：数据接受线程
 * 参数说明：
 *    Param：参数
 * 注意事项：null
 * 修改日期：2015/12/18 17:02:00
 ***********************************************************************************************************/
DWORD __stdcall IocpServerOperation::AcceptThread(LPVOID Param)
{
	IocpServerOperation* pthis = (IocpServerOperation*)Param;
	while (pthis->m_acceptThread)
	{
		SOCKET client;
		if ((client = accept(pthis->m_sSocket, NULL, NULL)) == INVALID_SOCKET)
		{
			/// 错误处理
		}
		pthis->OnAccept(client);																					/// 调用 OnAccept() 通知应用程序有新客户端
	}
	return 1;
}


/***********************************************************************************************************
 * 程序作者：赵进军
 * 函数功能：初始化
 * 参数说明：null
 * 注意事项：null
 * 修改日期：2015/12/18 17:12:00
 ***********************************************************************************************************/
bool IocpServerOperation::Init(void)
{
	if (bInit)
		return true;
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
		return false;

	bInit = true;
	return true;
}


/***********************************************************************************************************
 * 程序作者：赵进军
 * 函数功能：通讯监听
 * 参数说明：
 *     port：端口
 * 注意事项：null
 * 修改日期：2015/12/18 17:20:00
 ***********************************************************************************************************/
bool IocpServerOperation::Listen(int port)
{
	if (!bInit)
		if (!(Init()))
			return false;

	m_sSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sSocket == INVALID_SOCKET)
		return false;

	//SOCKADDR_IN addr;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//addr.sin_addr.S_un.S_addr = inet_addr(ip);

	if (bind(m_sSocket, (struct  sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		return false;

	if (listen(m_sSocket, 10) == SOCKET_ERROR)
		return false;

	if ((m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0)) == NULL)							/// 创建完成端口的句柄  
		return false;

	this->m_acceptThread = true;
	g_haThread = CreateThread(NULL, 0, AcceptThread, (LPVOID)this, 0, &m_athreadID);								/// 创建连接线程，用来接收客户端的连接


	this->m_workThread = true;
	g_hwThread = CreateThread(NULL, 0, WorkThread, (LPVOID)this, 0, &m_athreadID);									/// 创建工作线程，用来处理完成端口的消息
	return true;
}