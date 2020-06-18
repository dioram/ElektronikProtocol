#include <Encoder.h>
#include <sstream>

using namespace Elektronik::Data;

void writeHeader(std::stringstream& ss, const Header& header, const PackageType& pkgType, const ObjectType& objType) {
	ss
		.write((const char*)&pkgType, sizeof(PackageType))
		.write((const char*)&header.timestamp, sizeof(int))
		.write((const char*)&header.isKey, sizeof(bool))
		.write((const char*)&objType, sizeof(ObjectType))
		.write((const char*)&header.actionType, sizeof(ActionType));
}

void Encoder::GeneratePackage(const Header& header, const std::vector<SlamPoint>& points, std::vector<char>& data) {
	std::stringstream stream;
	writeHeader(stream, header, PackageType::SLAMPackage, ObjectType::Point);
	int pointsCount = (int)points.size();
	stream.write((const char*)&pointsCount, sizeof(int));
	for (const auto& pt : points) {
		stream.write((const char*)&pt.id, sizeof(int));
		switch (header.actionType)
		{
		case ActionType::Move:
			stream
				.write((const char*)pt.position.val, sizeof(float) * 3);
			break;
		case ActionType::Create:
			stream
				.write((const char*)pt.position.val, sizeof(float) * 3)
				.write((const char*)pt.defaultColor.val, sizeof(unsigned char) * 3)
				.write((const char*)pt.color.val, sizeof(unsigned char) * 3);
			break;
		case ActionType::Message:
		{
			int messageSize = pt.message.size();
			stream
				.write((const char*)&messageSize, sizeof(int))
				.write(pt.message.c_str(), messageSize);
			break; 
		}
		case ActionType::Tint:
			stream
				.write((const char*)pt.color.val, sizeof(unsigned char) * 3);
			break;
		default:
			break;
		}
	}
	const auto& str = stream.str();
	data.resize(str.size());
	std::memmove(data.data(), str.c_str(), data.size());
}

void Encoder::GeneratePackage(const Header& header, const std::vector<SlamObservation>& observations, std::vector<char>& data) {
	std::stringstream stream;
	writeHeader(stream, header, PackageType::SLAMPackage, ObjectType::Observation);
	int observationsCount = (int)observations.size();
	stream.write((const char*)&observationsCount, sizeof(int));
	for (const auto& obs : observations) {
		stream.write((const char*)&obs.point.id, sizeof(int));
		switch (header.actionType)
		{
		case ActionType::Move:
			stream
				.write((const char*)obs.point.position.val, sizeof(float) * 3)
				.write((const char*)obs.orientation.val, sizeof(float) * 4);
			break;
		case ActionType::Create:
			stream
				.write((const char*)obs.point.position.val, sizeof(float) * 3)
				.write((const char*)obs.orientation.val, sizeof(float) * 4)
				.write((const char*)obs.point.defaultColor.val, sizeof(unsigned char) * 3)
				.write((const char*)obs.point.color.val, sizeof(unsigned char) * 3);
			break;
		case ActionType::Message:
		{
			int messageSize = obs.point.message.size();
			stream
				.write((const char*)&messageSize, sizeof(int))
				.write(obs.point.message.c_str(), messageSize);
			break;
		}
		case ActionType::Tint:
			stream
				.write((const char*)obs.point.color.val, sizeof(unsigned char) * 3);
			break;
		default:
			break;
		}
	}
	const auto& str = stream.str();
	data.resize(str.size());
	std::memmove(data.data(), str.c_str(), data.size());
}

void Encoder::GeneratePackage(const Header& header, const std::vector<SlamConnection>& connections, const ObjectType& type, std::vector<char>& data) {
	std::stringstream stream;
	writeHeader(stream, header, PackageType::SLAMPackage, type);
	int connectionsCount = (int)connections.size();
	stream.write((const char*)&connectionsCount, sizeof(int));
	bool isCreateOrConnect = header.actionType == ActionType::Create || header.actionType == ActionType::Connect;
	for (const auto& c : connections) {
		stream.write((const char*)&c.pt1.id, sizeof(int));
		if (isCreateOrConnect || header.actionType == ActionType::Move)
			stream.write((const char*)c.pt1.position.val, sizeof(float) * 3);
		if (isCreateOrConnect)
			stream.write((const char*)c.pt1.defaultColor.val, sizeof(unsigned char) * 3);
		if (isCreateOrConnect || header.actionType == ActionType::Tint)
			stream.write((const char*)c.pt1.color.val, sizeof(unsigned char) * 3);

		stream.write((const char*)&c.pt2.id, sizeof(int));
		if (isCreateOrConnect || header.actionType == ActionType::Move)
			stream.write((const char*)c.pt2.position.val, sizeof(float) * 3);
		if (isCreateOrConnect)
			stream.write((const char*)c.pt2.defaultColor.val, sizeof(unsigned char) * 3);
		if (isCreateOrConnect || header.actionType == ActionType::Tint)
			stream.write((const char*)c.pt2.color.val, sizeof(unsigned char) * 3);
	}
	const auto& str = stream.str();
	data.resize(str.size());
	std::memmove(data.data(), str.c_str(), data.size());
}