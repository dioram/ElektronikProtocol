#include <Encoder.h>
#include <Networking/TCPServer.h>
#include <conio.h>
#include <fstream>

#define IN
#define OUT
#define INOUT

void create_points(
	OUT std::vector<Elektronik::Data::SlamPoint>& pts, 
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	std::vector<cv::Vec3f> positions;
	positions.push_back({ 0, 0, 0 });
	positions.push_back({ 0, 1, 0 });
	positions.push_back({ 1, 0, 0 });
	positions.push_back({ 0, -1, 0 });
	positions.push_back({ -1, 0, 0 });
	positions.push_back({ -.5f, .5f, 0 });
	positions.push_back({ .5f, .5f, 0 });
	positions.push_back({ .5f, -.5f, 0 });
	positions.push_back({ -.5f, -.5f, 0 });
	for (int i = 0; i < positions.size(); ++i) {
		pts.emplace_back(Elektronik::Data::SlamPoint(i, positions[i]));
	}
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Create;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, pts, res);
}

void message_points(
	IN const std::vector<Elektronik::Data::SlamPoint>& pts,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	std::vector<Elektronik::Data::SlamPoint> messagePts;
	messagePts.reserve(pts.size());
	for (int i = 0; i < pts.size(); ++i) {
		char buffer[256];
		sprintf_s(buffer, "Hello there! General Kenobi! Hallo! Wie geht denn so? %d", i);
		auto pt = pts[i];
		pt.message = std::string(buffer);
		messagePts.emplace_back(pt);
	}

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Message;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, messagePts, res);
}

void move_points(
	IN const std::vector<Elektronik::Data::SlamPoint>& pts,
	OUT std::vector<Elektronik::Data::SlamPoint>& movedPts,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	movedPts.clear();
	movedPts.reserve(pts.size());
	for (int i = 0; i < pts.size(); ++i) {
		auto pt = pts[i];
		movedPts.emplace_back(pt);
	}
	movedPts[0].position[1] += 0.1;

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Move;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, movedPts, res);
}

void tint_points(
	IN const std::vector<Elektronik::Data::SlamPoint>& pts,
	OUT std::vector<Elektronik::Data::SlamPoint>& tintedPts,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	tintedPts = { pts[0], pts[1], pts[2], pts[3], };
	for (auto& pt : tintedPts) {
		pt.color = { 0xff, 0, 0 };
	}
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Tint;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, tintedPts, res);
}

void connect_points(
	IN  const std::vector<Elektronik::Data::SlamPoint>& pts,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();

	std::vector<Elektronik::Data::SlamConnection> connections = {
			{ pts[1], pts[6] },
			{ pts[6], pts[2] },
			{ pts[2], pts[7] },
			{ pts[7], pts[3] },
			{ pts[3], pts[8] },
			{ pts[8], pts[4] },
			{ pts[4], pts[5] },
			{ pts[5], pts[1] },
			{ pts[5], pts[6] },
			{ pts[6], pts[7] },
			{ pts[7], pts[8] },
			{ pts[8], pts[5] },
			{ pts[0], pts[5] },
			{ pts[0], pts[6] },
			{ pts[0], pts[7] },
			{ pts[0], pts[8] },
		};
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Connect;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, connections, Elektronik::Data::ObjectType::Point, res);
}

void remove_points(
	IN const std::vector<Elektronik::Data::SlamPoint>& pts,
	OUT std::vector<Elektronik::Data::SlamPoint>& removedPts,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();

	removedPts = { pts[0] };

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Remove;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, removedPts, res);
}

void clear_points(
	OUT std::vector<char>& res,
	INOUT int& timestamp) {

	res.clear();
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Clear;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, std::vector<Elektronik::Data::SlamPoint>{}, res);

}

void test_points()
{
	printf("TEST: %s\n", __FUNCTION__"");
	TCPServer server;
	server.init("192.168.1.54", 12345);
	server.waitForClient(std::chrono::seconds(10000));
	std::vector<char> data;
	int timestamp = 0;
	std::vector<Elektronik::Data::SlamPoint> pts;

	create_points(pts, data, timestamp);
	int dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	connect_points(pts, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	std::vector<Elektronik::Data::SlamPoint> moved_pts;
	move_points(pts, moved_pts, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	std::vector<Elektronik::Data::SlamPoint> tinted_pts;
	tint_points(pts, tinted_pts, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	std::vector<Elektronik::Data::SlamPoint> removed_pts;
	remove_points(pts, removed_pts, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	message_points(pts, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	clear_points(data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
}

void create_observations(
	OUT std::vector<Elektronik::Data::SlamObservation>& obs, 
	OUT std::vector<char>& res, 
	INOUT int& timestamp) {
	std::vector<cv::Vec3f> positions = {
		{ 0, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 },
		{ 0, -1, 0 }, { -1, 0, 0 }, { -.5f, .5f, 0 }, 
		{ .5f, .5f, 0 }, { .5f, -.5f, 0 }, { -.5f, -.5f, 0 },
	};
	obs.clear();
	obs.reserve(positions.size());

	for (int i = 0; i < positions.size(); ++i) {
		obs.emplace_back(
			Elektronik::Data::SlamObservation(
				Elektronik::Data::SlamPoint(i, positions[i]), 
				{ 1, 0, 0, 0 }));
	}
	
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Create;
	header.isKey = true;
	header.timestamp = timestamp++;

	Elektronik::Data::Encoder::GeneratePackage(header, obs, res);
}

void message_observations(
	IN const std::vector<Elektronik::Data::SlamObservation>& obs,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	std::vector<Elektronik::Data::SlamObservation> messageObs;
	messageObs.reserve(obs.size());
	for (int i = 0; i < obs.size(); ++i) {
		char buffer[256];
		sprintf_s(buffer, "Hello there! General Kenobi! Hallo! Wie geht denn so? %d", i);
		auto ob = obs[i];
		ob.point.message = std::string(buffer);
		messageObs.emplace_back(ob);
	}

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Message;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, messageObs, res);
}

void connect_observations(
	IN const std::vector<Elektronik::Data::SlamObservation>& obs,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	std::vector<Elektronik::Data::SlamConnection> connections = {
			{ obs[1], obs[6] },
			{ obs[6], obs[2] },
			{ obs[2], obs[7] },
			{ obs[7], obs[3] },
			{ obs[3], obs[8] },
			{ obs[8], obs[4] },
			{ obs[4], obs[5] },
			{ obs[5], obs[1] },
			{ obs[5], obs[6] },
			{ obs[6], obs[7] },
			{ obs[7], obs[8] },
			{ obs[8], obs[5] },
			{ obs[0], obs[5] },
			{ obs[0], obs[6] },
			{ obs[0], obs[7] },
			{ obs[0], obs[8] },
	};
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Connect;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, connections, Elektronik::Data::ObjectType::Observation, res);
}

void move_observations(
	IN const std::vector<Elektronik::Data::SlamObservation>& observations,
	OUT std::vector<Elektronik::Data::SlamObservation>& movedObservations,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	movedObservations.clear();
	movedObservations.reserve(observations.size());
	for (int i = 0; i < observations.size(); ++i) {
		auto obs = observations[i];
		obs.point.position[2] += 1;
		obs.orientation = { 0, 1, 0, 0 };
		movedObservations.emplace_back(obs);
	}

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Move;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, movedObservations, res);
}

void remove_observations(
	IN const std::vector<Elektronik::Data::SlamObservation>& obs,
	OUT std::vector<Elektronik::Data::SlamObservation>& removed_obs,
	OUT std::vector<char>& res,
	INOUT int& timestamp) {
	res.clear();
	for (int i = 0; i < obs.size(); i += 2) {
		removed_obs.push_back(obs[i]);
	}
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Remove;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, removed_obs, res);
}

void clear_observations(
	OUT std::vector<char>& res,
	INOUT int& timestamp) {

	res.clear();
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Clear;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, std::vector<Elektronik::Data::SlamObservation>{}, res);

}

void test_observations() {
	printf("TEST: %s\n", __FUNCTION__"");
	TCPServer server;
	server.init("192.168.1.54", 12345);
	server.waitForClient(std::chrono::seconds(10000));
	std::vector<char> data;
	std::vector<Elektronik::Data::SlamObservation> obs;
	int timestamp = 0;

	create_observations(obs, data, timestamp);
	int dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	connect_observations(obs, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	std::vector<Elektronik::Data::SlamObservation> moved_obs;
	move_observations(obs, moved_obs, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();

	std::vector<Elektronik::Data::SlamObservation> obs2rm;
	remove_observations(obs, obs2rm, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
	clear_observations(data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
}

void create_lines(OUT std::vector<Elektronik::Data::SlamConnection>& lines, OUT std::vector<char>& res,  INOUT int& timestamp) {
	std::vector<Elektronik::Data::SlamPoint> pts;
	int stub = 0;
	create_points(pts, res, stub);
	res.clear();
	lines = {
		{ pts[1], pts[6] },
		{ pts[6], pts[2] },
		{ pts[2], pts[7] },
		{ pts[7], pts[3] },
		{ pts[3], pts[8] },
		{ pts[8], pts[4] },
		{ pts[4], pts[5] },
		{ pts[5], pts[1] },
		{ pts[5], pts[6] },
		{ pts[6], pts[7] },
		{ pts[7], pts[8] },
		{ pts[8], pts[5] },
		{ pts[0], pts[5] },
		{ pts[0], pts[6] },
		{ pts[0], pts[7] },
		{ pts[0], pts[8] },
	};

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Create;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, lines, Elektronik::Data::ObjectType::Connection, res);
}

void move_lines(INOUT std::vector<Elektronik::Data::SlamConnection>& lines, OUT std::vector<char>& res, INOUT int& timestamp) {
	for (auto& line : lines) {
		line.pt1.position[1] += 1;
		line.pt2.position[1] += 1;
	}
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Move;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, lines, Elektronik::Data::ObjectType::Connection, res);
}

void remove_lines(INOUT std::vector<Elektronik::Data::SlamConnection>& lines, OUT std::vector<char>& res, INOUT int& timestamp) {
	std::vector<Elektronik::Data::SlamConnection> lines2rm = {
		lines[0],
		lines[3],
		lines[7],
		lines[9],
	};

	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Remove;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, lines2rm, Elektronik::Data::ObjectType::Connection, res);
}

void clear_lines(OUT std::vector<char>& res, INOUT int& timestamp) {
	Elektronik::Data::Header header;
	header.actionType = Elektronik::Data::ActionType::Clear;
	header.isKey = true;
	header.timestamp = timestamp++;
	Elektronik::Data::Encoder::GeneratePackage(header, std::vector<Elektronik::Data::SlamConnection>{}, Elektronik::Data::ObjectType::Connection, res);
}

void test_lines() {
	printf("TEST: %s\n", __FUNCTION__"");
	TCPServer server;
	server.init("192.168.1.54", 12345);
	server.waitForClient(std::chrono::seconds(10000));
	std::vector<char> data;
	std::vector<Elektronik::Data::SlamConnection> lines;
	int timestamp = 0;
	create_lines(lines, data, timestamp);
	int dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
	move_lines(lines, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
	remove_lines(lines, data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
	clear_lines(data, timestamp);
	dataSize = data.size();
	server.sendMessage((const char*)&dataSize, sizeof(int));
	server.sendMessage(data.data(), data.size());
	_getch();
}

void write_test_file(const std::string& path) {
	const int MAGIC_NUMBER = 0xDEADBEEF;
	char MAGIC_NUMBER_BUFFER[4];
	std::memcpy(MAGIC_NUMBER_BUFFER, &MAGIC_NUMBER, sizeof(int));

	int offset = 0;
	std::vector<int> offsets;
	std::fstream file(path, std::ios::out | std::ios::binary);
	
	std::vector<Elektronik::Data::SlamPoint> pts;
	std::vector<char> data;
	int timestamp = 0;
	
	file.write(MAGIC_NUMBER_BUFFER, 4);
	offsets.push_back(offset += 4);

	create_points(pts, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	message_points(pts, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	connect_points(pts, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamPoint> moved_pts;
	move_points(pts, moved_pts, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamPoint> tinted_pts;
	tint_points(pts, moved_pts, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamPoint> removed_pts;
	remove_points(pts, removed_pts, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	clear_points(data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamObservation> obs;
	create_observations(obs, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	message_observations(obs, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	connect_observations(obs, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamObservation> moved_obs;
	move_observations(obs, moved_obs, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamObservation> removed_obs;
	remove_observations(obs, removed_obs, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	clear_observations(data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	std::vector<Elektronik::Data::SlamConnection> lines;
	create_lines(lines, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	move_lines(lines, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	remove_lines(lines, data, timestamp);
	file.write(data.data(), data.size());
	offsets.push_back(offset += data.size());

	clear_lines(data, timestamp);
	file.write(data.data(), data.size());
	offset += data.size();

	file.write(MAGIC_NUMBER_BUFFER, 4);

	int countOfPackages = offsets.size();
	char buffer[4];
	std::memcpy(buffer, &countOfPackages, sizeof(int));
	file.write(buffer, 4);

	for (int i = 0; i < offsets.size(); ++i) {
		std::memcpy(buffer, &offsets[i], sizeof(int));
		file.write(buffer, 4);
	}

	std::memcpy(buffer, &offset, sizeof(int));
	file.write(buffer, 4);
	file.flush();
	file.close();
}

int main(int argc, char* argv[]) {
	test_points();
	test_observations();
	test_lines();
	write_test_file("test.dat");
}