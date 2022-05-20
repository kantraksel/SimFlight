#include "LocalAirplane.h"

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
}

void LocalAirplane::DisableTransmission()
{
	isServerEnabled = false;
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
