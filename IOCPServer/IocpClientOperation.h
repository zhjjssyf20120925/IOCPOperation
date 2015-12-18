/**************************************************************************************************
 *    Copyright：An individual mandate_ZHJJSSYF
 * Create Class：赵进军
 *  Create Data：2015/12/18 16:33:00
 *  Description：IOCP 客户端实现
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

