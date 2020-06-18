#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
#include <string>

namespace Elektronik {
	namespace Data {

		enum class PackageType : unsigned char
		{
			SLAMPackage = 0,
			TrackingPackage = 1,
		};
		
		enum class ObjectType : unsigned char
		{
			Point = 0,
			Observation = 1,
			Connection = 2,
			Message = 3,
		};

		enum class ActionType : unsigned char
		{
			Create = 0,
			Tint = 1,
			Move = 2,
			Remove = 3,
			Fuse = 4,
			Connect = 5,
			Message = 6,
			Clear = 7,
		};

		struct SlamPoint
		{
			SlamPoint(int id) : id(id), position({ 0, 0, 0 }), defaultColor({ 0, 0, 0 }), color({ 0, 0, 0 }), message("") {};
			SlamPoint(int id, const std::string& message) : id(id), message(message), position({ 0, 0, 0 }), defaultColor({ 0, 0, 0 }), color({ 0, 0, 0 }) {};
			SlamPoint(int id, const cv::Vec3f& position) : id(id), position(position), defaultColor({ 0, 0, 0 }), color({ 0, 0, 0 }), message("") {};
			SlamPoint(int id, const cv::Vec3b& color, const cv::Vec3b& defaultColor = { 0, 0, 0 }) : color(color), defaultColor(defaultColor), position({ 0, 0, 0 }), message("") {};
			SlamPoint(int id, const cv::Vec3f& position, const cv::Vec3b& color, const cv::Vec3b& defaultColor = { 0, 0, 0 }, const std::string& message = "") :
				id(id), position(position), color(color), defaultColor(defaultColor), message(message) {};

			int id;
			cv::Vec3f position;
			cv::Vec3b defaultColor;
			cv::Vec3b color;
			std::string message;
		};

		struct SlamObservation
		{
			struct Stats
			{
				unsigned char statistics1;
				unsigned char statistics2;
				unsigned char statistics3;
				unsigned char statistics4;
			};

			struct CovisibleInfo
			{
				int id;
				int sharedPointsCount;
			};

			SlamPoint point;
			cv::Vec4f orientation;
			Stats statistics;

			SlamObservation(SlamPoint pt, cv::Vec4f orientation, Stats stats = Stats()) :
				point(pt), orientation(orientation), statistics(stats) { }

			operator SlamPoint&() { return this->point; }
			operator SlamPoint() const { return this->point; }
		};

		struct SlamConnection
		{
			SlamPoint pt1;
			SlamPoint pt2;
			SlamConnection(SlamPoint pt1, SlamPoint pt2) : pt1(pt1), pt2(pt2) {}
		};

		struct Header
		{
			bool isKey;
			int timestamp;
			ActionType actionType;
		};

		class Encoder
		{
		public:
			static void GeneratePackage(const Header& header, const std::vector<SlamPoint>& points, std::vector<char>& data);
			static void GeneratePackage(const Header& header, const std::vector<SlamObservation>& observations, std::vector<char>& data);
			static void GeneratePackage(const Header& header, const std::vector<SlamConnection>& connections, const ObjectType& type, std::vector<char>& data);
		};

	}
}