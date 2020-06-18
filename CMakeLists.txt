cmake_minimum_required(VERSION 3.15)
project(Elektronik_Data_SDK CXX)
set(PROJECT_LIB_NAME ElektronikDataEncoder)

set(HEADER_FILES 
	headers/Encoder.h
)
source_group("headers" FILES ${HEADER_FILES})

set(SOURCE_FILES src/Encoder.cpp)
source_group("sources" FILES ${SOURCE_FILES})

add_library(${PROJECT_LIB_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_LIB_NAME} PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/headers>
)

find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_LIB_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_LIB_NAME} PUBLIC ${OpenCV_LIBRARIES})

add_subdirectory(3rdparty/Networking)

add_executable(encoder_exec 
	src/main.cpp 
	# src/Tests/PointsTests.h 
	# src/Tests/PointsTests.cpp
)
target_link_libraries(encoder_exec PRIVATE ${PROJECT_LIB_NAME})
target_link_libraries(encoder_exec PRIVATE Networking)

find_package(GTest CONFIG REQUIRED)
target_link_libraries(encoder_exec PRIVATE GTest::gtest GTest::gtest_main)

add_custom_command(TARGET encoder_exec POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E 
		copy_directory
			$<TARGET_FILE_DIR:Networking>/
			$<TARGET_FILE_DIR:encoder_exec>/
)