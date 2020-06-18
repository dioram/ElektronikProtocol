#include "PointsTests.h"
#include <conio.h>

using namespace Elektronik::Data;

PointsTests::PointsTests() {
	m_server = std::make_shared<TCPServer>();
	m_timestamp = 0;
}

void draw(std::vector<Elektronik::Data::SlamPoint>& pts, std::vector<Elektronik::Data::SlamConnection>& connections) {
	std::vector<cv::Vec3f> positions =
	{
		{ 0, 0, 0 },     { 0, 1, 0 },      { 1, 0, 0 },
		{ 0, -1, 0 },    { -1, 0, 0 },     { -.5f, .5f, 0 },
		{ .5f, .5f, 0 }, { .5f, -.5f, 0 }, { -.5f, -.5f, 0 },
	};
	for (int i = 0; i < positions.size(); ++i) {
		Elektronik::Data::SlamPoint pt;
		pt.position = positions[i];
		pt.color = { 0, 0, 0 };
		pt.defaultColor = { 0, 0, 0 };
		pt.id = i;
		pt.message = "haha what a story, Mark?";
		pts.push_back(pt);
	}

	connections = {
		{ pts[1], pts[6] }, { pts[6], pts[2] }, { pts[2], pts[7] }, { pts[7], pts[3] },
		{ pts[3], pts[8] }, { pts[8], pts[4] }, { pts[4], pts[5] }, { pts[5], pts[1] },
		{ pts[5], pts[6] }, { pts[6], pts[7] }, { pts[7], pts[8] }, { pts[8], pts[5] },
		{ pts[0], pts[5] }, { pts[0], pts[6] }, { pts[0], pts[7] }, { pts[0], pts[8] },
	};
}


void PointsTests::SetUp() {
	m_server->init("192.168.1.54", 12345);
	m_server->waitForClient(std::chrono::seconds(10000));
	draw(m_initPoints, m_initConnections);
}

void PointsTests::TearDown() {
}

int PointsTests::NextTimestamp() {
	return m_timestamp++;
}

TEST_F(PointsTests, CreatePoints) {
	Header header;
	header.actionType = ActionType::Create;
	header.isKey = true;
	header.timestamp = NextTimestamp();
	std::vector<char> data;
	Encoder::GeneratePackage(header, m_initPoints, data);
	int dataSize = data.size();
	m_server->sendMessage((const char*)&dataSize, sizeof(int));
	m_server->sendMessage(data.data(), data.size());
	printf("enter y if elektronik behaviour is ok\n");
	char ch = _getch();
	ASSERT_EQ(ch, 'y');
}

TEST_F(PointsTests, ConnectPoints) {
	Header header;
	header.actionType = ActionType::Create;
	header.isKey = true;
	header.timestamp = NextTimestamp();
	std::vector<char> data;
	Encoder::GeneratePackage(header, m_initPoints, data);
	header.actionType = ActionType::Connect;
	header.timestamp = NextTimestamp();
	Encoder::GeneratePackage(header, m_initConnections, );
	printf("enter y if elektronik behaviour is ok\n");
	char ch = _getch();
	ASSERT_EQ(ch, 'y');
}

TEST_F(PointsTests, MovePoints) {
	int dataSize = data.size();
	m_server->sendMessage((const char*)&dataSize, sizeof(int));
	m_server->sendMessage(data.data(), data.size());
	char ch = _getch();
	ASSERT_EQ(ch, 'y');
}

TEST_F(PointsTests, RemovePoints) {
	int dataSize = data.size();
	m_server->sendMessage((const char*)&dataSize, sizeof(int));
	m_server->sendMessage(data.data(), data.size());
	char ch = _getch();
	ASSERT_EQ(ch, 'y');
}

TEST_F(PointsTests, ClearPoints) {
	int dataSize = data.size();
	m_server->sendMessage((const char*)&dataSize, sizeof(int));
	m_server->sendMessage(data.data(), data.size());
	char ch = _getch();
	ASSERT_EQ(ch, 'y');
}