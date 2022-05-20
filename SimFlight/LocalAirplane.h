#pragma once
#include "AirplaneData.h"
#include "FGServer.h"
#include "SCClient.h"

class LocalAirplane : public SCClient::AirplaneEvents
{
	private:
		AirplaneData data;
		FGServer server;
		bool isServerEnabled;

	public:
		LocalAirplane();
		FGServer& GetServer();

		void OnTick();
		void EnableTransmission();
		void DisableTransmission();

		virtual void OnFlightStart();
		virtual void OnFlightEnd();
		virtual void OnUpdate(const AirplaneData& data);
};
