#include "LocalAirplane.h"
#include "Logger.h"

LocalAirplane::LocalAirplane()
{
	isServerEnabled = false;
}

FGServer& LocalAirplane::GetServer()
{
	return server;
}

void LocalAirplane::OnTick()
{
	if (isServerEnabled)
		server.ProcessTick(data);
}

void LocalAirplane::EnableTransmission()
{
	isServerEnabled = true;
	Logger::Log("Airplane: broadcast enabled");
}

void LocalAirplane::DisableTransmission()
{
	isServerEnabled = false;
	Logger::Log("Airplane: broadcast disabled");
}

void LocalAirplane::OnFlightStart()
{
	EnableTransmission();
}

void LocalAirplane::OnFlightEnd()
{
	DisableTransmission();
}

void LocalAirplane::OnUpdate(const AirplaneData& data)
{
	this->data = data;
}
