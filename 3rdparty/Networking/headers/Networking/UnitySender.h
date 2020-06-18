#pragma once

#include <Networking/IServer.h>

class NETWORKING_EXPORT UnitySender
{
public:
	~UnitySender();
	UnitySender(IServer* server);
	//Send send full data package to choosen endpoint
	Error SendPackage(DataType x, DataType y, DataType z,
		DataType qw, DataType qx, DataType qy, DataType qz,
		unsigned int nPnts, DataType *arPnts);
	
	//Send custom package to choosen endpoint
	Error SendCustomPackage(void* data, size_t length);

	//Send pose to choosen endpoint
	Error SendPose(
		DataType qx, DataType qy, DataType qz, DataType qw,
		DataType x, DataType y, DataType z
	);

	IServer* GetServer();

private:
	unsigned int _messageCount = 0;
	IServer* m_server;
};

