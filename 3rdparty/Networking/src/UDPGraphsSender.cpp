#include <Networking/UDPGraphsSender.h>

Error UDPGraphsSender::sendPoint(Point3D point)
{
	return sendMessage(point.toString());
}

Error UDPGraphsSender::sendAllData(
	Point3D robPos, Point3D odoPos,
	Point3D dltRobPos, Point3D dltOdoPos,
	Point3D tsPoint)
{
	std::string graphsData;
	graphsData.append(robPos.toString() + ";");
	graphsData.append(odoPos.toString() + ";");
	graphsData.append(dltRobPos.toString() + ";");
	graphsData.append(dltOdoPos.toString() + ";");
	graphsData.append(tsPoint.toString());
	return sendMessage(graphsData);
}