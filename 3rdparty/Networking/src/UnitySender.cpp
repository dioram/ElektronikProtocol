#include <Networking/UnitySender.h>
#include <cstring>

UnitySender::UnitySender(IServer* server) : m_server(server)
{
}

UnitySender::~UnitySender()
{
	if (m_server != nullptr)
		delete m_server;
}

Error UnitySender::SendPackage(
	DataType x, DataType y, DataType z,
	DataType qw, DataType qx, DataType qy, DataType qz,
	unsigned int nPnts, DataType *arPnts)
{
	size_t offset = 8;
	size_t nData = 3 * nPnts;
	float* data = new float[offset + nData];
	data[0] = x; data[1] = y; data[2] = z;
	data[3] = qw; data[4] = qx; data[5] = qy; data[6] = qz;
	data[7] = nPnts;

	for (int i = 0; i < nData; i += 3)
	{
		data[i + offset] = arPnts[i];
		data[i + offset + 1] = arPnts[i + 1];
		data[i + offset + 2] = arPnts[i + 2];
	}
	Error msgResult = m_server->sendMessage((const char*)data, (8 + nData) * sizeof(float));
	delete[] data;
	return msgResult;
}

Error UnitySender::SendCustomPackage(void* data, size_t length)
{
	const char* fullData = new char[length + sizeof(int)];
	//memcpy((void *)fullData, &_messageCount, sizeof(int));
	std::memcpy((void *)fullData, &length, sizeof(int));
    std::memcpy((void *)(fullData + sizeof(int)), data, length);

	Error msgResult = m_server->sendMessage(fullData, length + sizeof(int));
	delete[] fullData;
	_messageCount++;
	return msgResult;
}

Error UnitySender::SendPose(
	DataType qx, DataType qy, DataType qz, DataType qw,
	DataType x, DataType y, DataType z)
{
	return SendPackage(x, y, z, qw, qz, qy, qx, 0, nullptr);
}

IServer* UnitySender::GetServer()
{
	return m_server;
}
