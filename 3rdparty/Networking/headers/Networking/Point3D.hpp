#pragma once

#include "Macro.h"
#include <Networking/export.h>
#include <string>

class NETWORKING_EXPORT Point3D
{
private:
	DataType m_x;
	DataType m_y;
	DataType m_z;
public:
	DataType getX();
	DataType getY();
	DataType getZ();
	std::string toString();
	Point3D(DataType x, DataType y, DataType z);
};

inline DataType Point3D::getX()
{
	return m_x;
}

inline DataType Point3D::getY()
{
	return m_y;
}

inline DataType Point3D::getZ()
{
	return m_z;
}

inline std::string Point3D::toString()
{
	std::string stringPoint;
	stringPoint.append(std::to_string(m_x) + ":");
	stringPoint.append(std::to_string(m_y) + ":");
	stringPoint.append(std::to_string(m_z));
	return stringPoint;
}

inline Point3D::Point3D(DataType x, DataType y, DataType z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}
