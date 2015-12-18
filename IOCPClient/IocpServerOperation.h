/**************************************************************************************************
 *    Copyright��An individual mandate_ZHJJSSYF
 * Create Class���Խ���
 *  Create Data��2015/12/18 16:00:00
 *  Description��IOCP �����ʵ����
 **************************************************************************************************/
#pragma once
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
const int OP_READ = 0;
const int OP_WRITE = 1;
const int OP_ACCEPT = 2;

/*
OVERLAPPEDPLUS �ṹ�����˼·
OVERLAPPED ��һ���̶������ڴ���������Ϣ�¼�����ֵ�Ľṹ�����
����ɶ˿ں��ص�I/Oģ�������ڷ�����Ϣ�¼��Ľ��
��Ϊ�ڴ���������Ϣ��ʱ�򣬷��͵���һ������ֵ�Ľṹ��ָ�룬ֻҪ�ṹ��
��ǰ�沿������ϵͳ��Ҫ����ϵͳ�����ɹ���ʱ��Ҳ�ͻ������ṹ��ָ��
���ظ��û�������ֻҪ��ϵͳ����Ľṹ�������չһЩ�Լ��Ķ������Ϳ���
�����ɵ�ȷ������Ϣ��˭�������ġ�
����������ɶ˿�����Ƶ�ʱ��Ҳ����������������������������ֻ�Ƿ���
һЩ��ϵͳ�����йص����ݣ��û���Ҫ��ŵ���������Ͳ��ڴ��
����洢��ϵͳ��ص������У�
socket
OpCode ������Ϣ�Ĳ������ͣ�����ɶ˿ڵĲ������棬������Ϣ֪ͨϵͳ��
������/д���ݣ�����Ҫ����������Ϣ�ṹ���ȥ�ģ��������ϵͳҪͬʱ
���ж�д�����Ļ�������Ҫ��һ�����������ֲ����ˣ�

WSABUF   wbuf;                  //   ��д�������ṹ�����
DWORD    dwBytes, dwFlags; //   һЩ�ڶ�дʱ�õ��ı�־�Ա���
char buf[4096];                  //   �Լ��Ļ�����
�����4��������ŵ���һЩ����Ϣ��ص����ݣ�����һЩ�������õ��ģ�
��Щ�������ǹ̶��ģ�����������Ҫ�ο�һ����ɶ˿���غ����Ĳ����ӿ�
*/

struct OVERLAPPEDPLUS
{
	OVERLAPPED             ol;
	SOCKET				   s;
	int			           OpCode;
	WSABUF				   wbuf;
	DWORD                  dwBytes, dwFlags;
	char                   buf[4096];
};

class IocpServerOperation
{
public:
	bool					m_workThread;
	bool					m_acceptThread;
	HANDLE					m_hIocp;																				// ��ɶ˿ھ��
	SOCKET					m_sSocket;

	IocpServerOperation();
	~IocpServerOperation();

	virtual void OnRead(void* p, char* buf, int len){};
	virtual void OnAccept(SOCKET socket);
	virtual void OnClose(void* p){};

	/// ��һ��socket��һ���Զ���Ľṹ���������ɶ˿�(�൱�ڰ�socket��һ���ṹ��������а�),
	/// ��������������3�������¼���ʱ�򣬸ýṹ��������ٴ��ظ�����
	/// �����Ϳ������ֵ�ǰ�����¼����Ǹ�socket������
	bool SetIoCompletionPort(SOCKET socket, void* p, char* buf = NULL, int len = 0);

	bool Init(void);

	bool Listen(int port);

	static DWORD __stdcall WorkThread(LPVOID Param);

	static DWORD __stdcall AcceptThread(LPVOID Param);
protected:
	HANDLE					g_hwThread;																				// �����߳̾��
	DWORD					m_wthreadID;
	HANDLE					g_haThread;																				// �����߳̾��
	DWORD					m_athreadID;
private:
};

