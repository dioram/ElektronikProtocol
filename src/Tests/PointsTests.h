#pragma once
#include <gtest/gtest.h>
#include <Networking/TCPServer.h>
#include <Encoder.h>

class PointsTests : public ::testing::Test {
protected:
	PointsTests();
	void SetUp() override;
	void TearDown() override;

	int NextTimestamp();

	std::shared_ptr<TCPServer> m_server;
	std::vector<Elektronik::Data::SlamPoint> m_initPoints;
	std::vector<Elektronik::Data::SlamConnection> m_initConnections;

private:
	int m_timestamp;
};

