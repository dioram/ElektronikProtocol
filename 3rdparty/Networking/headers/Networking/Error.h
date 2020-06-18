#pragma once

#include <Networking/Macro.h>
#include <Networking/export.h>

enum NETWORKING_EXPORT Error
{
	invalid = -1,
	success = 0,
	wrongIPAddress,
	endpointWasntSet,
	UDPSocketWasntCreated,
	sendError,
};