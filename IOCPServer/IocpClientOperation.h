/**************************************************************************************************
 *    Copyright��An individual mandate_ZHJJSSYF
 * Create Class���Խ���
 *  Create Data��2015/12/18 16:33:00
 *  Description��IOCP �ͻ���ʵ��
 **************************************************************************************************/
#pragma once
#include<winsock.h>
class IocpClientOperation
{
public:
	IocpClientOperation();
	~IocpClientOperation();
	bool Connect(char* ip, int port);
	void Send(char* buf, int len);
protected:
	SOCKET m_socket;
};

