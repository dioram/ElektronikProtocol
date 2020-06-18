project(OMW.Networking CXX)
set(${PROJECT_NAME}_LIB_NAME Networking)

################################################################################
# Source groups
################################################################################

set(Header_Files
    headers/${${PROJECT_NAME}_LIB_NAME}/Error.h
    headers/${${PROJECT_NAME}_LIB_NAME}/IServer.h
	headers/${${PROJECT_NAME}_LIB_NAME}/ITCPServer.h
    headers/${${PROJECT_NAME}_LIB_NAME}/Macro.h
    headers/${${PROJECT_NAME}_LIB_NAME}/Point3D.hpp
    headers/${${PROJECT_NAME}_LIB_NAME}/TCPServer.h
    headers/${${PROJECT_NAME}_LIB_NAME}/UDPGraphsSender.h
    headers/${${PROJECT_NAME}_LIB_NAME}/UDPServer.h
    headers/${${PROJECT_NAME}_LIB_NAME}/UnitySender.h
)
source_group("headers" FILES ${Header_Files})

set(Source_Files
    src/TCPServer.cpp
    src/UDPGraphsSender.cpp
    src/UDPServer.cpp
    src/UnitySender.cpp
)
source_group("sources" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################

add_library(${${PROJECT_NAME}_LIB_NAME} SHARED ${ALL_FILES})

include(GenerateExportHeader)
generate_export_header(${${PROJECT_NAME}_LIB_NAME}
	EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/${${PROJECT_NAME}_LIB_NAME}/export.h
)

################################################################################
# Target tuning
################################################################################

set_target_properties(${${PROJECT_NAME}_LIB_NAME} 
	PROPERTIES
		CXX_STANDARD 17
		CXX_EXTENSIONS OFF
)

target_compile_definitions(${${PROJECT_NAME}_LIB_NAME} PRIVATE
    "UNICODE;"
    "_UNICODE;"
	"UDP_SENDER_LIBRARY_EXPORTS;"
)

target_include_directories(${${PROJECT_NAME}_LIB_NAME}
	PUBLIC
		$<BUILD_INTERFACE:
			${CMAKE_CURRENT_BINARY_DIR}/include
			${CMAKE_CURRENT_SOURCE_DIR}/headers>
)

################################################################################
# External libraries
################################################################################

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS date_time exception timer)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${${PROJECT_NAME}_LIB_NAME} PUBLIC ${Boost_LIBRARIES})