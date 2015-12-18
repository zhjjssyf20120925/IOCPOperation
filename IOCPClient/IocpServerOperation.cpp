#include "stdafx.h"
#include "IocpServerOperation.h"

static bool bInit = false;


/***********************************************************************************************************
* �������ߣ��Խ���
* �������ܣ����캯��
* ����˵����null
* ע�����null
* �޸����ڣ�2015/12/18 16:37:00
***********************************************************************************************************/
IocpServerOperation::IocpServerOperation()
{
}


/***********************************************************************************************************
* �������ߣ��Խ���
* �������ܣ���������
* ����˵����null
* ע�����null
* �޸����ڣ�2015/12/18 16:38:00
***********************************************************************************************************/
IocpServerOperation::~IocpServerOperation()
{
}


/***********************************************************************************************************
 * �������ߣ��Խ���
 * �������ܣ������߳�
 * ����˵����
 *    Param��ָ�����͵Ĳ���
 * ע�����null
 * �޸����ڣ�2015/12/18 16:45:00
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
			pthis->OnClose(re);																						/// �ͻ��˶Ͽ�����
			OVERLAPPEDPLUS* olp = (OVERLAPPEDPLUS*)pOverlap;
			closesocket(olp->s);
			delete olp;																								/// �ͷ���socket�󶨵Ľṹ�����
			continue;
		}

		if (NULL == re)
			return 0;
		OVERLAPPEDPLUS *olp = (OVERLAPPEDPLUS*)pOverlap;
		switch (olp->OpCode)
		{
		case OP_READ:
			pthis->OnRead(re, olp->wbuf.buf, berByte);																/// ���� Onread() ֪ͨӦ�ó��򣬷������յ����Կͻ��˵���������
			WSARecv(olp->s, &olp->wbuf, 1, &olp->dwBytes, &olp->dwFlags, &olp->ol, NULL);							/// ��������һ�����ܵ�I/O�첽����
			break;
		default:
			break;
		}
	}
	return 0;
}


/***********************************************************************************************************
 * �������ߣ��Խ���
 * �������ܣ����ݽ����߳�
 * ����˵����
 *    Param������
 * ע�����null
 * �޸����ڣ�2015/12/18 17:02:00
 ***********************************************************************************************************/
DWORD __stdcall IocpServerOperation::AcceptThread(LPVOID Param)
{
	IocpServerOperation* pthis = (IocpServerOperation*)Param;
	while (pthis->m_acceptThread)
	{
		SOCKET client;
		if ((client = accept(pthis->m_sSocket, NULL, NULL)) == INVALID_SOCKET)
		{
			/// ������
		}
		pthis->OnAccept(client);																					/// ���� OnAccept() ֪ͨӦ�ó������¿ͻ���
	}
	return 1;
}


/***********************************************************************************************************
 * �������ߣ��Խ���
 * �������ܣ���ʼ��
 * ����˵����null
 * ע�����null
 * �޸����ڣ�2015/12/18 17:12:00
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
 * �������ߣ��Խ���
 * �������ܣ�ͨѶ����
 * ����˵����
 *     port���˿�
 * ע�����null
 * �޸����ڣ�2015/12/18 17:20:00
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

	if ((m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0)) == NULL)							/// ������ɶ˿ڵľ��  
		return false;

	this->m_acceptThread = true;
	g_haThread = CreateThread(NULL, 0, AcceptThread, (LPVOID)this, 0, &m_athreadID);								/// ���������̣߳��������տͻ��˵�����


	this->m_workThread = true;
	g_hwThread = CreateThread(NULL, 0, WorkThread, (LPVOID)this, 0, &m_athreadID);									/// ���������̣߳�����������ɶ˿ڵ���Ϣ
	return true;
}