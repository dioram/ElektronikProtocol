#pragma once

#include <Networking/UDPServer.h>
#include <Networking/Point3D.hpp>

class NETWORKING_EXPORT UDPGraphsSender : public UDPServer
{
public:
	Error sendPoint(Point3D point);
	Error sendAllData(Point3D robPos, Point3D odoPos,
		Point3D dltRobPos, Point3D dltOdoPos,
		Point3D tsPoint);
};
