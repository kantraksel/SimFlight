#include "SocketApi.h"
#include "WNET/include/wnet.h"

void SocketApi::Init()
{
	WNET::Subsystem::Initialize();
}

void SocketApi::Shutdown()
{
	WNET::Subsystem::Release();
}
